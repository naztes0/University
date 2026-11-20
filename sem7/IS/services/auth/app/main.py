import asyncio
import html
import os
import time
from contextlib import asynccontextmanager

from fastapi import Cookie, FastAPI, Header, HTTPException, Request, Response, status
from fastapi.responses import HTMLResponse, JSONResponse
from pydantic import BaseModel, Field

from .database import AuthDatabase
from .security import (
    DUMMY_PASSWORD_HASH,
    hash_password,
    hash_token,
    new_token,
    tokens_match,
    validate_username,
    verify_password,
)

SESSION_COOKIE = "charging_session"
CSRF_COOKIE = "charging_csrf"
LOGIN_CSRF_COOKIE = "charging_login_csrf"
UNSAFE_METHODS = {"POST", "PUT", "PATCH", "DELETE"}

database = AuthDatabase()
session_ttl = int(os.getenv("AUTH_SESSION_TTL_SECONDS", "28800"))
cookie_secure = os.getenv("AUTH_COOKIE_SECURE", "false").lower() == "true"
admin_username = validate_username(os.environ["AUTH_ADMIN_USERNAME"])
admin_password = os.environ["AUTH_ADMIN_PASSWORD"]
if len(admin_password) < 12:
    raise RuntimeError("AUTH_ADMIN_PASSWORD must contain at least 12 characters")


class LoginRequest(BaseModel):
    username: str = Field(min_length=3, max_length=64)
    password: str = Field(min_length=1, max_length=256)


@asynccontextmanager
async def lifespan(_: FastAPI):
    database.initialize()
    if not database.has_users():
        password_hash = await asyncio.to_thread(hash_password, admin_password)
        database.create_initial_user(admin_username, password_hash)
    yield


app = FastAPI(
    title="Charging Station Authentication",
    version="0.1.0",
    lifespan=lifespan,
    docs_url=None,
    redoc_url=None,
    openapi_url=None,
)


def set_cookie(
    response: Response,
    key: str,
    value: str,
    *,
    max_age: int,
    httponly: bool,
) -> None:
    response.set_cookie(
        key=key,
        value=value,
        max_age=max_age,
        httponly=httponly,
        secure=cookie_secure,
        samesite="lax",
        path="/",
    )


def get_session(session_token: str | None):
    if not session_token:
        return None
    return database.get_session(hash_token(session_token))


@app.get("/auth/healthz")
async def health() -> dict[str, str]:
    return {"status": "ok"}


@app.get("/auth/login", response_class=HTMLResponse)
async def login_page() -> HTMLResponse:
    csrf_token = new_token()
    database.create_login_challenge(hash_token(csrf_token), int(time.time()) + 600)
    escaped_token = html.escape(csrf_token, quote=True)
    page = f"""<!doctype html>
<html lang="uk">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Вхід — Charging Monitor</title>
  <style>
    body {{ font-family: system-ui, sans-serif; max-width: 26rem; margin: 12vh auto; padding: 1rem; }}
    form {{ display: grid; gap: .8rem; }}
    input, button {{ font: inherit; padding: .7rem; }}
    #message {{ min-height: 1.4rem; color: #b42318; }}
  </style>
</head>
<body data-login-csrf="{escaped_token}">
  <h1>Вхід до системи</h1>
  <form id="login-form">
    <label>Користувач <input name="username" autocomplete="username" required></label>
    <label>Пароль <input name="password" type="password" autocomplete="current-password" required></label>
    <button type="submit">Увійти</button>
    <div id="message" role="alert"></div>
  </form>
  <script src="/auth/login.js" defer></script>
</body>
</html>"""
    response = HTMLResponse(page)
    response.headers["Content-Security-Policy"] = (
        "default-src 'self'; script-src 'self'; style-src 'self' 'unsafe-inline'; "
        "img-src 'self' data:; connect-src 'self'; frame-ancestors 'self'"
    )
    set_cookie(
        response,
        LOGIN_CSRF_COOKIE,
        csrf_token,
        max_age=600,
        httponly=True,
    )
    return response


@app.get("/auth/login.js")
async def login_script() -> Response:
    script = """
const form = document.querySelector('#login-form');
const message = document.querySelector('#message');
form.addEventListener('submit', async (event) => {
  event.preventDefault();
  message.textContent = '';
  const data = new FormData(form);
  const response = await fetch('/auth/login', {
    method: 'POST',
    credentials: 'same-origin',
    headers: {
      'Content-Type': 'application/json',
      'X-CSRF-Token': document.body.dataset.loginCsrf
    },
    body: JSON.stringify({ username: data.get('username'), password: data.get('password') })
  });
  if (response.ok) {
    window.location.assign('/');
    return;
  }
  const payload = await response.json().catch(() => ({}));
  message.textContent = payload.detail || 'Не вдалося виконати вхід';
});
"""
    return Response(script, media_type="application/javascript")


@app.post("/auth/login")
async def login(
    payload: LoginRequest,
    request: Request,
    x_csrf_token: str | None = Header(default=None),
    login_csrf_cookie: str | None = Cookie(default=None, alias=LOGIN_CSRF_COOKIE),
) -> JSONResponse:
    if not tokens_match(x_csrf_token, login_csrf_cookie):
        raise HTTPException(status_code=status.HTTP_403_FORBIDDEN, detail="Invalid CSRF token")
    login_challenge_hash = hash_token(x_csrf_token)
    if not database.validate_login_challenge(login_challenge_hash):
        raise HTTPException(status_code=status.HTTP_403_FORBIDDEN, detail="Expired CSRF token")

    try:
        username = validate_username(payload.username)
    except ValueError:
        username = payload.username.strip().lower()[:64]
    client_key = request.headers.get("x-real-ip") or request.client.host
    window_start = int(time.time()) - 300
    if database.count_recent_failures(client_key, username, window_start) >= 5:
        response = JSONResponse(
            status_code=status.HTTP_429_TOO_MANY_REQUESTS,
            content={"detail": "Too many login attempts. Try again later."},
        )
        response.headers["Retry-After"] = "300"
        return response

    user = database.get_user(username)
    password_hash = user["password_hash"] if user else DUMMY_PASSWORD_HASH
    password_valid = await asyncio.to_thread(
        verify_password,
        password_hash,
        payload.password,
    )
    if user is None or not password_valid:
        database.record_login_failure(client_key, username)
        raise HTTPException(
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail="Invalid username or password",
        )

    database.clear_login_failures(client_key, username)
    database.delete_login_challenge(login_challenge_hash)
    session_token = new_token()
    csrf_token = new_token()
    database.create_session(
        hash_token(session_token),
        int(user["id"]),
        hash_token(csrf_token),
        int(time.time()) + session_ttl,
    )
    response = JSONResponse({"username": user["username"], "expires_in": session_ttl})
    set_cookie(response, SESSION_COOKIE, session_token, max_age=session_ttl, httponly=True)
    set_cookie(response, CSRF_COOKIE, csrf_token, max_age=session_ttl, httponly=False)
    response.delete_cookie(LOGIN_CSRF_COOKIE, path="/")
    return response


@app.get("/auth/verify")
async def verify_session(
    x_original_method: str = Header(default="GET"),
    x_csrf_check: str = Header(default="0"),
    x_csrf_token: str | None = Header(default=None),
    session_token: str | None = Cookie(default=None, alias=SESSION_COOKIE),
) -> Response:
    session = get_session(session_token)
    if session is None:
        return Response(status_code=status.HTTP_401_UNAUTHORIZED)
    if x_csrf_check == "1" and x_original_method.upper() in UNSAFE_METHODS:
        if not x_csrf_token or not tokens_match(
            hash_token(x_csrf_token),
            session["csrf_hash"],
        ):
            return Response(status_code=status.HTTP_403_FORBIDDEN)
    return Response(
        status_code=status.HTTP_204_NO_CONTENT,
        headers={"X-Auth-User": session["username"]},
    )


@app.get("/auth/session")
async def session_info(
    session_token: str | None = Cookie(default=None, alias=SESSION_COOKIE),
) -> dict[str, object]:
    session = get_session(session_token)
    if session is None:
        raise HTTPException(status_code=status.HTTP_401_UNAUTHORIZED, detail="Not authenticated")
    return {"username": session["username"], "expires_at": session["expires_at"]}


@app.post("/auth/logout")
async def logout(
    x_csrf_token: str | None = Header(default=None),
    session_token: str | None = Cookie(default=None, alias=SESSION_COOKIE),
) -> Response:
    session = get_session(session_token)
    if session is None:
        raise HTTPException(status_code=status.HTTP_401_UNAUTHORIZED, detail="Not authenticated")
    if not x_csrf_token or not tokens_match(hash_token(x_csrf_token), session["csrf_hash"]):
        raise HTTPException(status_code=status.HTTP_403_FORBIDDEN, detail="Invalid CSRF token")
    database.delete_session(hash_token(session_token))
    response = Response(status_code=status.HTTP_204_NO_CONTENT)
    response.delete_cookie(SESSION_COOKIE, path="/")
    response.delete_cookie(CSRF_COOKIE, path="/")
    return response
