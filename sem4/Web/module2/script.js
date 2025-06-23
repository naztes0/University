
let alarms = [];

function addAlarm() {
    const alarmTime = document.getElementById("setAlarm")
    const time = new Date(alarmTime.value);
    if (isNaN(time)) {
        alert("Введіть корректний час")
        return;
    }
    const id = Date.now()
    alarms.push({ id, time, active: true });
    displayAlarms();
    alarmTime.value = "";
}

function displayAlarms() {
    const list = document.getElementById("alarmsList");
    list.innerHTML = "<h3>Список будильників</h3>";

    alarms.forEach(alarm => {
        const item = document.createElement("div");
        item.className = "alarm-item";
        const timeContainer = document.createElement("div")

        timeContainer.classList.add("timeContainer")
        const alarmTime = document.createElement("h4")
        const alarmDate = document.createElement("p")
        alarmTime.textContent = alarm.time.toLocaleTimeString();
        alarmDate.textContent = alarm.time.toLocaleDateString()
        timeContainer.appendChild(alarmTime)
        timeContainer.appendChild(alarmDate)
        item.appendChild(timeContainer)
        const isTurned = document.createElement("span")
        isTurned.textContent = alarm.active ? "увімкнено" : "вимкнено";

        item.appendChild(isTurned)
        const buttonContianer = document.createElement("div")

        const toggleButton = document.createElement("button");
        toggleButton.textContent = alarm.active ? "Вимкнути" : "Увімкнути";
        toggleButton.addEventListener("click", () => toggleAlarm(alarm.id));

        const deleteButton = document.createElement("button");
        deleteButton.textContent = "Видалити";
        deleteButton.addEventListener("click", () => removeAlarm(alarm.id));

        buttonContianer.appendChild(toggleButton)
        buttonContianer.appendChild(deleteButton)

        item.appendChild(buttonContianer)
        list.appendChild(item);
    });
}

function toggleAlarm(id) {
    for (let i = 0; i < alarms.length; i++) {
        if (alarms[i].id === id) {
            alarms[i].active = !alarms[i].active;
            break;
        }
    }
    displayAlarms();
}

function removeAlarm(id) {
    for (let i = 0; i < alarms.length; i++) {
        if (alarms[i].id === id) {
            alarms.splice(i, 1);
            break;
        }
    }
    displayAlarms();
}


setInterval(() => {
    const now = new Date();
    alarms.forEach(alarm => {
        if (
            alarm.active &&
            Math.abs(now - alarm.time) < 1000
        ) {
            alert("Будильник! Час: " + alarm.time.toLocaleTimeString());
            alarm.active = false;
            displayAlarms();
        }
    });
}, 1000);
