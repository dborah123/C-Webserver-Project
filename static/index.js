async function getMessage() {
    /**
     * Gets previous function user's message from server
     */
    console.log("henlo")
    const response = await fetch("/json_message", {
        'headers': {
            'Accept': 'application/json'
        }
    })
    const message = await response.json()
    console.log(message)

    const messageBox = document.getElementById("message-box");

    messageBox.innerHTML = `
        <div>
            ${message["name"]}
        </div>
    `
}


displayInfo = function() {
    /**
     * Returns info that user submitted
     */
    const infoBox = document.getElementById("info-box")

    // Retrieving user's submitted information
    const name = document.getElementById("name")
    const age = document.getElementById("age")
    const email = document.getElementById("email")

    infoBox.innerHTML = `
        <div class="alert alert-info" role="alert">
            Hello ${name}... your submitted age is ${age} and email is ${email}
        </div>
    `
}


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
