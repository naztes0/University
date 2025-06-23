
////TASK1////

//func decl
let fizz = "fizz"
let buzz = "buzz"
function fizzbuzz(num) {
    for (let i = 1; i <= num; i++) {
        if (i % 3 === 0 && i % 5 === 0) {
            console.log(fizz + buzz);
        } else if (i % 3 === 0) {
            console.log(fizz);
        } else if (i % 5 === 0) {
            console.log(buzz);
        } else {
            console.log(i);
        }
    }
}

fizzbuzz(15)

////////Task 2//////
console.log("TASK 2")
function cost(a, b) {
    let grn = "гривень"
    let kop = "копійок"

    if (b >= 100) {
        a += (b - (b % 100)) / 100;
        b = b % 100;
    }

    if (a % 10 === 1) grn = "гривня"
    if (a % 10 === 2) grn = "гривні"
    if (2 <= b % 10 && b % 10 <= 4) kop = "копійки"
    if (b % 10 === 1) kop = "копійка"



    console.log(`Вартість замовлення: ${a} ${grn}  ${b} ${kop}`)
}

console.log("cost(2,75)")
cost(2, 75)
console.log("cost(0,1)")
cost(0, 1)
console.log("cost(25,3)")
cost(25, 3)
console.log("cost(20,102)")
cost(20, 102)

console.log("TASK 3")
function point(x, y) {
    return [x, y];

}

function getQuarter(p) {
    const [x, y] = p;
    if (x > 0 && y > 0) return 1;
    if (x < 0 && y > 0) return 2;
    if (x < 0 && y < 0) return 3;
    if (x > 0 && y < 0) return 4;
    if (x === 0 && y === 0) return ' on the (0;0)';
    if (x === 0) return 'On Y-axis';
    if (y === 0) return 'On X-axis';
}

function vector(p1, p2) {
    const q1 = getQuarter(p1)
    const q2 = getQuarter(p2)

    if (q1 === q2 && typeof (q1) === "number") {
        let nx = p2[0] - p1[0]
        let ny = p2[1] - p1[1]
        const dist = Math.sqrt(nx * nx + ny * ny)
        console.log(`The distant between [${[] = p1}] and [${[] = p2}] is ${dist}`)
        return dist
    }
    else {
        if (q1 !== q2 && (typeof (q1) === "number" && typeof (q2) === "number")) {
            console.log(`Point [${[] = p1}] is in the ${q1} quarter and point [${[] = p2}] is in the ${q2} quarter`)
        }
        else {
            if (q1 !== q2 && (typeof (q1) === "number" && typeof (q2) === "number")) {
                console.log(`Point [${[] = p1}] is in the ${q1} quarter and point [${[] = p2}] is in the ${q2} quarter`)
            }
            else {
                if (typeof (q1) !== "number") {
                    console.log(`The point [${[] = p1}] is ${q1}`)
                }
                if (typeof (q2) !== "number") {
                    console.log(`The point [${[] = p2}] is ${q2}`)

                }
            }
        }
    }

}

let point1 = point(3, 2)
let point2 = point(6, 6)
let point3 = point(-1, -2)
let point4 = point(5, 0)
let point5 = point(0, 0)

vector(point1, point2)
vector(point1, point3)
vector(point2, point4)
vector(point3, point5)
vector(point4, point5)
