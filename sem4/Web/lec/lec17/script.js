
marksGrid = document.querySelector(".marks")
diagram = document.querySelector(".diagram")

const categories = ["Sport", "Food", "Detergents", "Cothes"]

const sells = [
    {
        category: "Sport",
        sells: 52,
    },
    {
        category: "Food",
        sells: 98,
    },
    {
        category: "Detergents",
        sells: 38,
    },
    {
        category: "Clothes",
        sells: 158,
    }
]



//creation of categories and adding the columns to dia

const createColumn = (sells, maxSells, color) => {
    const categCol = document.createElement("div")
    categCol.classList.add("column")
    const newCol = document.createElement("div");
    newCol.style.width = "100%";
    newCol.style.backgroundColor = color
    newCol.style.height = `${(sells / maxSells) * 100}%`;
    const sellsNumber = document.createElement("p")
    sellsNumber.textContent = sells;

    categCol.appendChild(newCol)
    categCol.appendChild(sellsNumber)
    diagram.appendChild(categCol)

}
const maxSells = () => {
    let maxSells = 0
    for (let i = 0; i < sells.length; i++) {
        if (sells[i].sells > maxSells) maxSells = sells[i].sells
    }
    return maxSells;
}

for (let i = 0; i < sells.length; i++) {
    const newCateg = document.createElement("div")
    newCateg.classList.add("mark")

    const colorCirc = document.createElement("div")
    const randomColor = `#${Math.floor(Math.random() * 16777215).toString(16).padStart(6, "0")}`
    colorCirc.classList.add("colCirc")
    colorCirc.style.backgroundColor = randomColor

    const categName = document.createElement("h5")
    categName.textContent = sells[i].category;

    newCateg.appendChild(colorCirc)
    newCateg.appendChild(categName)
    marksGrid.appendChild(newCateg);



    createColumn(sells[i].sells, maxSells(), randomColor)
}



