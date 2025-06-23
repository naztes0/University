

const arr1 = [1, 2, 3, 7, 4, 5];
const arr2 = [4, 5, 6, 7, 8, 9, 10];
const arr3 = [-4, -8, -2, -1, 0, 1, 4, 5, 6, 9, 16, 5];
const letters = ["o", "d", "e", "r", "a", "n"];

///1
console.log("---1---")
console.log(`Array1:${[] = arr1}`)
console.log(`Array2:${[] = arr2}`)
const combined = arr1.concat(arr2);
const unique = []
combined.forEach(function (numb) {
    if (!unique.includes(numb)) {
        unique.push(numb);
    }
})

console.log(`Unique numbers: ${[] = unique}`)


///2
console.log("---2---")
console.log(`Array:${[] = arr3}`)
let even = arr3.filter(function (num) {
    return num % 2 === 0 && num > 0;
});
console.log(`Even numbers > 0: ${[] = even}`);


///3
console.log("---3---")
console.log(`Array:${[] = arr3}`)
const find = arr3.some(num => num === 0);
if (find) {
    console.log(`Found 0 in arr3 at index ${arr3.indexOf(0)}`);
}


//4
console.log("---4---")
console.log(`Array:${[] = arr3}`)
const find1 = arr3.some(num => num === 5);
if (find) {
    console.log(`Found 5 in array at index ${arr3.indexOf(5)}`);
}


//5
console.log("---5---")
console.log(`Array:${[] = arr1}`)
let dob = 1;
arr1.forEach(function (num) {
    dob *= num;
})
console.log(`Multiplication of arr1: ${dob}`)



//6
console.log("---6---")
console.log(`Array:${[] = letters}`)
console.log(letters.sort())

console.log(`Array:${[] = arr3}`)
console.log(arr3.sort((a, b) => Math.abs(a) - Math.abs(b)))