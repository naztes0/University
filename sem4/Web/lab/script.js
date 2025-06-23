
const slides = document.querySelectorAll(".slide");
const dots = document.querySelectorAll(".dot");
const prev = document.querySelector(".prev");
const next = document.querySelector(".next");
const feedback = document.querySelector(".feedback")
const feedbackWind = document.querySelector(".feedback-container")
const overlay = document.querySelector(".overlay");
const closeWind = document.querySelector(".close-window")

let current = 0;
let timer;
let timerInterval = 4; //in seconds


// FEEDBACK
feedback.addEventListener('click', (e) => {
    e.preventDefault();
    overlay.style.display = 'block';
    feedbackWind.classList.add("show")
})
closeWind.addEventListener('click', () => {
    overlay.style.display = 'none'
    feedbackWind.classList.remove("show")
})
overlay.addEventListener('click', () => {
    if (overlay.style.display === 'block') {
        overlay.style.display = 'none'
        feedback.classList.remove("show");
        feedbackWind.classList.remove("show")
    }
})


//SLIDER
function showSlide(index) {
    for (let i = 0; i < slides.length; i++) {
        slides[i].classList.toggle("active", i === index)
        dots[i].classList.toggle("active", i === index)
    }
    current = index;
}

function nextSlide() {
    const next = (current + 1) % slides.length
    showSlide(next)
}

function prevSlide() {
    const prev = (current - 1 + slides.length) % slides.length
    showSlide(prev)
}

function startAutoSlide() {
    timer = setInterval(nextSlide, timerInterval * 1000);
}

function stopAutoSlide() {
    clearInterval(timer);
}

next.addEventListener('click', (e) => {
    e.preventDefault();
    stopAutoSlide();
    nextSlide();
    startAutoSlide();
})

prev.addEventListener('click', (e) => {
    e.preventDefault();
    stopAutoSlide();
    prevSlide();
    startAutoSlide();
})

dots.forEach(dot => {
    dot.addEventListener('click', e => {
        stopAutoSlide();
        showSlide(Number(e.target.dataset.index));
        startAutoSlide();
    })
})

showSlide(current);
startAutoSlide();

//PAGINATION
const itemsPerPage = 6;
const items = Array.from(document.querySelectorAll(".items_container .item")); //choose all elemnts from items container to arr
const pagination = document.getElementById("pagination");

//Counting the page qty 
const totalPages = Math.ceil(items.length / itemsPerPage);


function showPage(page) {
    const start = (page - 1) * itemsPerPage;
    const end = page * itemsPerPage;
    items.forEach((item, index) => {
        item.style.display = (index >= start && index < end) ? "block" : "none";
    });

    //Chawnge status of btn as active page
    Array.from(pagination.children).forEach(btn => btn.classList.remove("active"));
    pagination.children[page - 1].classList.add("active");
}

//Creating pagination in dynamic way
function createPagination() {
    for (let i = 1; i <= totalPages; i++) {
        const btn = document.createElement("button");
        btn.textContent = i;
        btn.addEventListener("click", () => showPage(i));
        pagination.appendChild(btn);
    }
}

createPagination();
showPage(1);


