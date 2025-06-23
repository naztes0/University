
const container = document.querySelector(".countryInf")
const comboBox = document.getElementById("boxCountries")


const countries = {
    japan: {
        country: "Японія",
        flag: "https://upload.wikimedia.org/wikipedia/commons/9/9e/Flag_of_Japan.svg",
        sights: [
            {
                img: "https://img.pac.ru/landmarks/422705/big/70C788687F00010143B9384C8F4598AF.jpg",
                name: "Храм Кінкаку-дзі",
                price: "165 UAH",
                description: "Знаменитий золотий павільйон у Кіото, побудований в 14 столітті"
            },
            {
                img: "https://vsviti.com.ua/wp-content/uploads/2011/09/himejijo-3.jpg",
                name: "Замок Хімедзі",
                price: "350 UAH",
                description: "Один з найкраще збережених японських замків, внесений до списку ЮНЕСКО"
            },
            {
                img: "https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcRcrjzIB2IbVPcFdjmxEY6OMyNmsnAHFLF1fA&s",
                name: "Фудзіяма",
                price: "Безкоштовно",
                description: "Символ Японії, популярне місце для походів"
            }
        ]
    },
    italy: {
        country: "Італія",
        flag: "https://upload.wikimedia.org/wikipedia/en/0/03/Flag_of_Italy.svg",
        sights: [
            {
                img: "https://upload.wikimedia.org/wikipedia/commons/thumb/d/de/Colosseo_2020.jpg/960px-Colosseo_2020.jpg",
                name: "Колізей",
                price: "400 UAH",
                description: "Стародавній амфітеатр у Римі"
            },
            {
                img: "https://ua.topvoyager.com/wp-content/uploads/2020/01/leaning-tower-of-pisa-1.jpg",
                name: "Пізанська вежа",
                price: "300 UAH",
                description: "Всесвітньо відома похила вежа"
            },
            {
                img: "https://lh4.googleusercontent.com/proxy/6CoOSl-cPYABr1VgjxxTmF3FyGbzUOoIRdKHr_OfpH2NjV4UnpTyaBoSv0P1naF8jUh6F045czF5jgbl0cdLetkjn-jZFHAGSbGejOYuuVlF1Pler55ehw",
                name: "Флорентійський собор",
                price: "200 UAH",
                description: "Архітектурна перлина епохи Відродження"
            }
        ]
    },
    brazil: {
        country: "Бразилія",
        flag: "https://upload.wikimedia.org/wikipedia/en/0/05/Flag_of_Brazil.svg",
        sights: [
            {
                img: "https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcT0Ulh7EkyKGr2M0eqIY1AfLWU5QAyifgt5wQ&s",
                name: "Статуя Христа-Спасителя",
                price: "500 UAH",
                description: "Одна з нових семи чудес світу в Ріо-де-Жанейро"
            },
            {
                img: "https://cdn4.suspilne.media/images/resize/1020x1.78/045134e171a49eec.jpg",
                name: "Амазонські тропіки",
                price: "Безкоштовно",
                description: "Найбільший тропічний ліс у світі"
            },
            {
                img: "https://www.stadium.odessa.ua/wp-content/uploads/2021/04/%D0%BC%D0%B0%D1%803.jpg",
                name: "Стадіон Маракана",
                price: "250 UAH",
                description: "Легендарний футбольний стадіон у Ріо"
            }
        ]
    },

    china: {
        country: "Китай",
        flag: "https://upload.wikimedia.org/wikipedia/commons/f/fa/Flag_of_the_People%27s_Republic_of_China.svg",
        sights: [
            {
                img: "https://turmandry.com.ua/tm/wp-content/uploads/2017/09/3.jpg",
                name: "Великий Китайський мур",
                price: "600 UAH",
                description: "Одна з найбільших архітектурних споруд у світі"
            },
            {
                img: "https://www.thepalacemuseumstationery.com/wp-content/uploads/2023/12/111.webp",
                name: "Заборонене місто",
                price: "350 UAH",
                description: "Імператорський палац у центрі Пекіна"
            },
            {
                img: "https://naked-science.ru/wp-content/uploads/2017/08/field_image_terracotta.army_.original.17159.jpg",
                name: "Терракотова армія",
                price: "400 UAH",
                description: "Поховна армія першого імператора Китаю"
            }
        ]
    },

    egypt: {
        country: "Єгипет",
        flag: "https://upload.wikimedia.org/wikipedia/commons/f/fe/Flag_of_Egypt.svg",
        sights: [
            {
                img: "https://trips.com.ua/wp-content/uploads/2022/11/giza-pyramids.jpg",
                name: "Піраміди Гізи",
                price: "500 UAH",
                description: "Один з найвідоміших архітектурних ансамблів світу"
            },
            {
                img: "https://turpoisk.ua/images/blog/sphinx/the-great-sphinx-giza.jpg",
                name: "Сфінкс",
                price: "300 UAH",
                description: "Містична статуя з тілом лева та головою людини"
            },
            {
                img: "https://pohcdn.com/sites/default/files/styles/paragraph__hero_banner__hb_image__1880bp/public/hero_banner/karnak-temple.jpg",
                name: "Храм Карнак",
                price: "250 UAH",
                description: "Один з найстаріших храмових комплексів"
            }
        ]
    }

};

comboBox.addEventListener("change", () => {
    container.innerHTML = "";

    const selectedCountry = countries[comboBox.value];

    const namedFlag = document.createElement("div");
    namedFlag.style.width = "100%";
    namedFlag.style.aspectRatio = "20/ 9";
    namedFlag.style.position = "relative";
    namedFlag.style.backgroundImage = `url('${selectedCountry.flag}')`;
    namedFlag.style.backgroundSize = "cover";
    namedFlag.style.backgroundPosition = "center";
    namedFlag.style.marginBottom = "20px";
    namedFlag.style.overflow = "hidden";

    const overlay = document.createElement("div");
    overlay.style.position = "absolute";
    overlay.style.top = "0";
    overlay.style.left = "0";
    overlay.style.width = "100%";
    overlay.style.height = "100%";
    overlay.style.backgroundColor = "rgba(0, 0, 0, 0.64)";

    const title = document.createElement("h2");
    title.textContent = selectedCountry.country;
    title.style.position = "absolute";
    title.style.top = "50%";
    title.style.left = "50%";
    title.style.transform = "translate(-50%, -50%)";
    title.style.color = "white";
    title.style.fontSize = "4em";
    title.style.fontWeight = "bold";
    title.style.textAlign = "center";
    title.style.margin = "0";

    namedFlag.appendChild(overlay);
    namedFlag.appendChild(title);
    container.appendChild(namedFlag);

    // SIGHTS
    const sightsContainer = document.createElement("div");
    sightsContainer.classList.add("sights-container");

    const selectedIndexes = new Set();

    const updateTotal = () => {
        let total = 0;
        selectedIndexes.forEach(index => {
            const priceStr = selectedCountry.sights[index].price;
            if (priceStr.toLowerCase().includes("безкоштовно")) return;
            const numeric = parseFloat(priceStr);
            if (!isNaN(numeric)) total += numeric;
        });
        totalContainer.textContent = `Загальна вартість: ${total} UAH`;
    };

    selectedCountry.sights.forEach((sightData, index) => {
        const sight = document.createElement("div");
        sight.classList.add("sight");
        sight.style.backgroundImage = `url(${sightData.img})`;


        const sightOverlay = document.createElement("div");
        sightOverlay.classList.add("sight-overlay")


        const name = document.createElement("h3");
        name.textContent = sightData.name;
        name.style.margin = "0";
        name.style.fontSize = "1.4rem";

        const desc = document.createElement("p");
        desc.textContent = sightData.description;
        desc.style.margin = "5px 0";
        desc.style.fontSize = "0.9rem";

        const price = document.createElement("p");
        price.textContent = `Ціна: ${sightData.price}`;
        price.style.margin = "0";
        price.style.fontWeight = "bold";
        price.style.fontSize = "1rem";

        sightOverlay.appendChild(name);
        sightOverlay.appendChild(desc);
        sightOverlay.appendChild(price);
        sight.appendChild(sightOverlay);

        sight.addEventListener("click", () => {
            if (selectedIndexes.has(index)) {
                selectedIndexes.delete(index);
                sight.classList.remove("selected");
            } else {
                selectedIndexes.add(index);
                sight.classList.add("selected");
            }
            updateTotal();
        });

        sightsContainer.appendChild(sight);
    });

    container.appendChild(sightsContainer);

    const totalContainer = document.createElement("div");
    totalContainer.classList.add("total-container");
    totalContainer.textContent = "Загальна вартість: 0 UAH";
    container.appendChild(totalContainer);
});

