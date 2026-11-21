const $ = (s) => document.querySelector(s);
const labels = {
  charging: "Заряджає",
  idle: "Очікує",
  offline: "Без зв’язку",
  error: "Помилка",
  historical: "Вимкнені",
  unknown: "Без даних",
};
let current = null,
  stations = [],
  filter = "all",
  busy = false,
  initialized = false,
  dbOnline = false,
  selected = null;
const csrf = () =>
  document.cookie
    .split("; ")
    .find((x) => x.startsWith("charging_csrf="))
    ?.split("=")
    .slice(1)
    .join("=") || "";
function notice(text, error = false) {
  $("#notice").hidden = !text;
  $("#notice").textContent = text;
  $("#notice").classList.toggle("error", error);
}
async function request(path, method = "GET", data) {
  const controller = new AbortController();
  const timer = setTimeout(() => controller.abort(), 20000);
  try {
    const r = await fetch(path, {
      method,
      credentials: "same-origin",
      signal: controller.signal,
      headers: { "Content-Type": "application/json", "X-CSRF-Token": csrf() },
      body: data ? JSON.stringify(data) : undefined,
    });
    if (r.status === 401) {
      location.assign("/auth/login");
      throw Error("Сесія завершилась. Увійдіть повторно.");
    }
    if (!r.ok) {
      const p = await r.json().catch(() => ({}));
      throw Error(
        r.status === 403
          ? "Не вдалося перевірити запит. Оновіть сторінку."
          : r.status === 503
            ? "PostgreSQL недоступний. Перевірте стан БД."
            : r.status === 409
              ? "Генератор уже працює."
              : r.status === 422
                ? "Перевірте допустимі межі всіх параметрів."
                : typeof p.detail === "string"
                  ? p.detail
                  : "Сервіс тимчасово недоступний.",
      );
    }
    return r.status === 204 ? null : await r.json();
  } finally {
    clearTimeout(timer);
  }
}
function config() {
  const f = $("#config-form").elements;
  return {
    station_count: Number(f.station_count.value),
    interval_ms: Math.round(Number(f.interval_seconds.value) * 1000),
    duration_seconds: Number(f.duration_seconds.value),
    max_concurrency: Number(f.max_concurrency.value),
  };
}
function updateControls() {
  $("#start").disabled = busy || !current || current.running || !dbOnline;
  $("#stop").disabled = busy || !current?.running;
  $("#demo-error").disabled = busy || !dbOnline;
}
function renderStatus(s) {
  current = s;
  if (!initialized) {
    const f = $("#config-form").elements;
    for (const k of ["station_count", "duration_seconds", "max_concurrency"])
      f[k].value = s.config[k];
    f.interval_seconds.value = s.config.interval_ms / 1000;
    initialized = true;
    estimate();
  }
  $("#requested").textContent =
    s.requested_readings_per_second.toLocaleString("uk-UA");
  $("#actual").textContent = s.actual_readings_per_second.toLocaleString(
    "uk-UA",
    { maximumFractionDigits: 1 },
  );
  $("#success").textContent = s.successful_readings.toLocaleString("uk-UA");
  $("#errors").textContent = s.failed_operations.toLocaleString("uk-UA");
  $("#errors").style.color = s.failed_operations ? "var(--red)" : "";
  const states = {
    running: "Працює",
    completed: "Завершено",
    stopped: "Зупинено",
    failed: "Помилка",
    interrupted: "Перервано",
  };
  $("#run-state").textContent = states[s.run_status] || "Очікування";
  $("#run-state").className = "badge" + (s.running ? " good" : "");
  const t = Math.floor(s.elapsed_seconds);
  $("#elapsed").textContent =
    String(Math.floor(t / 60)).padStart(2, "0") +
    ":" +
    String(t % 60).padStart(2, "0");
  $("#run-id").textContent = s.run_id || "Ще не запускався";
  updateControls();
}
function isStale(s) {
  const measuredAt = Date.parse(s.measured_at);
  const startedAt = Date.parse(current?.started_at);
  return (
    !!s.measured_at &&
    (!dbOnline ||
      !current?.running ||
      !current.started_at ||
      s.id > current.config.station_count ||
      measuredAt < startedAt ||
      Date.now() - measuredAt >
        Math.max(15000, current.config.interval_ms * 3))
  );
}

function stationCategory(s) {
  if (!s.measured_at) return "unknown";
  if (isStale(s)) return "historical";
  return s.status || "unknown";
}

function renderStations() {
  const visible = stations.filter(
    (s) => filter === "all" || stationCategory(s) === filter,
  );
  $("#station-count").textContent = visible.length;
  $("#empty").hidden = visible.length !== 0;
  const fragment = document.createDocumentFragment();
  for (const s of visible) {
    const button = document.createElement("button");
    const state = s.status || "unknown";
    const historical = isStale(s);
    const stateText = historical
      ? `Вимкнено · було: ${labels[state]}`
      : labels[state];
    button.className = `station ${state}${historical ? " stale" : ""}`;
    button.dataset.id = s.id;
    button.setAttribute("aria-label", `${s.code}, ${stateText}`);
    button.innerHTML =
      '<span class="station-top"><span class="code"></span><span class="station-icon" aria-hidden="true">ϟ</span></span><strong></strong><small></small><span class="stale-label"></span>';
    button.querySelector(".code").textContent = s.code;
    button.querySelector("strong").textContent =
      s.power_kw === null ? "—" : `${s.power_kw.toFixed(1)} кВт`;
    button.querySelector("small").textContent = stateText;
    button.querySelector(".stale-label").textContent = historical
      ? "Останні збережені дані"
      : "";
    fragment.append(button);
  }
  $("#stations").replaceChildren(fragment);
  for (const b of document.querySelectorAll("[data-filter]")) {
    const k = b.dataset.filter;
    const n =
      k === "all"
        ? stations.length
        : stations.filter((s) => stationCategory(s) === k).length;
    b.textContent = (k === "all" ? "Усі" : labels[k]) + ` · ${n}`;
    b.classList.toggle("selected", filter === k);
  }
  if (selected !== null) showDetail(selected, false);
}
function showDetail(id, open = true) {
  const s = stations.find((x) => x.id === id);
  if (!s) return;
  selected = id;
  $("#detail-title").textContent = s.code;
  const dl = document.createElement("dl");
  dl.className = "detail-list";
  for (const [name, value] of [
    ["Стан", labels[s.status || "unknown"]],
    ["Потужність", s.power_kw === null ? "—" : s.power_kw + " кВт"],
    ["Напруга", s.voltage_v === null ? "—" : s.voltage_v + " В"],
    ["Струм", s.current_a === null ? "—" : s.current_a + " А"],
    ["Температура", s.temperature_c === null ? "—" : s.temperature_c + " °C"],
    [
      "Енергія за інтервал",
      s.energy_delta_kwh === null ? "—" : s.energy_delta_kwh + " кВт·год",
    ],
  ]) {
    const box = document.createElement("div");
    const dt = document.createElement("dt");
    dt.textContent = name;
    const dd = document.createElement("dd");
    dd.textContent = value;
    box.append(dt, dd);
    dl.append(box);
  }
  const p = document.createElement("p");
  p.className = "muted hint";
  p.textContent = s.measured_at
    ? `Виміряно: ${new Date(s.measured_at).toLocaleString("uk-UA")} · ${isStale(s) ? "станція вимкнена" : "поточний запуск"}`
    : "Вимірювань ще немає.";
  $("#detail-body").replaceChildren(dl, p);
  if (open) $("#detail").showModal();
}
async function refresh() {
  const results = await Promise.allSettled([
    request("/api/load/status"),
    request("/api/stations"),
  ]);
  if (results[0].status === "fulfilled") renderStatus(results[0].value);
  else {
    current = null;
    notice("Немає зв’язку із застосунком. Повторюємо підключення…", true);
  }
  dbOnline = results[1].status === "fulfilled";
  $("#connection").textContent = dbOnline
    ? "● PostgreSQL доступний"
    : "● БД недоступна";
  $("#connection").className = "badge " + (dbOnline ? "good" : "bad");
  if (dbOnline) {
    stations = results[1].value;
    $("#updated").textContent =
      "Оновлено " + new Date().toLocaleTimeString("uk-UA");
  }
  $("#fleet-note").textContent = !dbOnline
    ? "БД недоступна. Показано останній отриманий знімок."
    : current?.running
      ? "Поточні статуси рахуються лише для станцій активного запуску."
      : "Навантаження не працює. Станції сірі, але останні показники збережені.";
  renderStations();
  updateControls();
}
async function act(fn) {
  busy = true;
  updateControls();
  try {
    await fn();
  } catch (e) {
    notice(
      e.name === "AbortError"
        ? "Запит триває довше очікуваного. Стан буде перевірено автоматично."
        : e.message,
      true,
    );
  } finally {
    busy = false;
    await refresh();
  }
}
function estimate() {
  const c = config();
  $("#estimate").textContent = Number.isFinite(
    (c.station_count * 1000) / c.interval_ms,
  )
    ? `${((c.station_count * 1000) / c.interval_ms).toFixed(1)} вимірювань / с за цими параметрами`
    : "Укажіть параметри";
}
$("#config-form").addEventListener("input", estimate);
$("#config-form").addEventListener("submit", (e) => {
  e.preventDefault();
  if (!$("#config-form").reportValidity()) return;
  act(async () => {
    await request("/api/load/config", "PUT", config());
    renderStatus(await request("/api/load/start", "POST"));
    notice("Генерацію запущено.");
  });
});
$("#stop").addEventListener("click", () =>
  act(async () => {
    notice("Завершуємо поточні операції…");
    renderStatus(await request("/api/load/stop", "POST"));
    notice("Генерацію зупинено.");
  }),
);
$("#demo-error").addEventListener("click", () =>
  act(async () => {
    const result = await request("/api/demo/failed-write", "POST");
    notice(
      `Тестову операцію відхилено PostgreSQL. trace_id: ${result.trace_id}. Лог і повну трасу можна знайти в Grafana.`,
    );
  }),
);
$("#logout").addEventListener("click", () =>
  act(async () => {
    await request("/auth/logout", "POST");
    location.assign("/auth/login");
  }),
);
$(".filters").addEventListener("click", (e) => {
  const b = e.target.closest("[data-filter]");
  if (b) {
    filter = b.dataset.filter;
    renderStations();
  }
});
$("#stations").addEventListener("click", (e) => {
  const b = e.target.closest("[data-id]");
  if (b) showDetail(Number(b.dataset.id));
});
$("#close-detail").addEventListener("click", () => $("#detail").close());
$("#detail").addEventListener("close", () => (selected = null));
request("/auth/session")
  .then((s) => ($("#username").textContent = s.username))
  .catch((e) => notice(e.message, true));
async function poll() {
  try {
    await refresh();
  } catch (e) {
    notice("Не вдалося оновити дані.", true);
  }
  setTimeout(poll, 2000);
}
poll();
