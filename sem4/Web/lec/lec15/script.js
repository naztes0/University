
const visitTimeEl = document.getElementById('visitTime');
const now = new Date();

const days = ['Неділя', 'Понеділок', 'Вівторок', 'Середа', 'Четвер', 'Пʼятниця', 'Субота'];

const formatted = `Дата та час: ${now.toLocaleString('uk-UA')} День тижня: ${days[now.getDay()]}`;

visitTimeEl.textContent = formatted;

let startTime = Date.now();

let timer = 0;
setInterval(() => {
    timer++;
    const hours = Math.floor(timer / 3600);
    const minutes = Math.floor(timer / 60) % 60;
    const seconds = timer % 60;
    document.getElementById("duration").textContent =
        `Час на сторінці:${hours}год ${minutes} хв ${seconds} сек`;
}, 1000);

