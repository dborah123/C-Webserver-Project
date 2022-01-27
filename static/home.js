console.log("home.js")
const btn = document.getElementById("btn-0")

btn.addEventListener("click", () => {
    const textBox = document.getElementById("text-box")
    textBox.innerHTML = `
        <div>
            This button was pressed
        </div>
    `
})
