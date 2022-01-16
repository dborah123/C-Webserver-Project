function handleAlerts(type, msg, num) {
    /**
     * Takes in type of alert, message, and id number of alertbox 
     * and returns a bootstrap alert
     */

    const alertBox = document.getElementById("alert-box-${num}")
    alertBox.innerHTML = `
        <div class="alert alert-${type} role="alert">
            ${msg}
        </div>
    `
}