const form = document.querySelector("#login-form");
const message = document.querySelector("#message");
form.addEventListener("submit", async (event) => {
  event.preventDefault();
  const button = form.querySelector("button");
  button.disabled = true;
  message.textContent = "";
  try {
    const data = new FormData(form);
    const response = await fetch("/auth/login", {
      method: "POST",
      credentials: "same-origin",
      headers: {
        "Content-Type": "application/json",
        "X-CSRF-Token": document.body.dataset.loginCsrf,
      },
      body: JSON.stringify({
        username: data.get("username"),
        password: data.get("password"),
      }),
    });
    if (response.ok) {
      location.assign("/");
      return;
    }
    message.textContent =
      response.status === 401
        ? "Неправильне ім’я користувача або пароль."
        : response.status === 429
          ? "Забагато спроб. Спробуйте через 5 хвилин."
          : response.status === 403
            ? "Час дії форми завершився. Оновіть сторінку."
            : "Не вдалося увійти. Перевірте введені дані.";
  } catch {
    message.textContent = "Сервер недоступний. Спробуйте ще раз.";
  } finally {
    button.disabled = false;
  }
});
