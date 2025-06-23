
/*DIAGRAM*/
const diagram = document.querySelector(".diagram")
const inputField = document.getElementById("colNumb")
const createDiaBtn = document.querySelector(".createDia")




createDiaBtn.addEventListener("click", () => {
    diagram.innerHTML = "";
    const colNumb = parseInt(inputField.value)
    if (!colNumb) return

    for (let i = 1; i <= colNumb; i++) {
        const newCol = document.createElement("div");

        const height = `${Math.floor(Math.random() * 100) + 50}px`;
        const color = `#${Math.floor(Math.random() * 16777215).toString(16).padStart(6, "0")}`;

        newCol.style.width = "30px";
        newCol.style.height = height;
        newCol.style.backgroundColor = color;

        diagram.appendChild(newCol)
    }
})

/*TABLE*/

const tabRow = document.getElementById("tabRow")
const tabCol = document.getElementById("tabCol")
const place = document.querySelector(".chessTable")
const buttonCreateTab = document.querySelector(".createTab")

const cellStyle = {
    black: "black",
    white: "white",
}

buttonCreateTab.addEventListener("click", () => {

    place.innerHTML = "";

    const row = parseInt(tabRow.value);
    const col = parseInt(tabCol.value);
    console.log(row, col)

    if (0 > row) {
        tabRow.value = "";
        tabRow.placeholder = "Enter number more than 0";
        return;
    }

    if (0 > col) {
        tabCol.value = "";
        tabCol.placeholder = "Enter number more than 0";
        return;
    }

    const table = document.createElement("table")
    for (let i = 0; i < row; i++) {
        const newTabRow = document.createElement("tr")
        for (let j = 0; j < col; j++) {
            const newRowCell = document.createElement("td");
            if ((i + j) % 2 === 0) newRowCell.style.backgroundColor = cellStyle.black;
            else newRowCell.style.backgroundColor = cellStyle.white;
            newRowCell.style.height = "20px";
            newRowCell.style.width = "20px";
            newTabRow.appendChild(newRowCell)
        }
        table.appendChild(newTabRow);
    }

    place.appendChild(table)
})


