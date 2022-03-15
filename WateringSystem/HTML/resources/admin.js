/* Import main functions, variables from main.js */
import { warningColor, errorColor, okColor, scalePage, getvals, CustomDialog } from '/resources/main.js';

/* Global variables */
var serverIP;   // IP address of server
var serverURL;  // URL of server
var page;

var dialog = new CustomDialog();

$(document).ready(function () {
    $('.navbar').load("./resources/navbar.htm");

    scalePage();

    // Gets ip address of watering system server when it is connected to the internet 
    let ipRequestUrl = "https://api.ipify.org?format=json";

    getvals(ipRequestUrl).then(response => {
        serverURL = $(location).attr('hostname');
        if (response !== undefined) {
            //console.log(response.ip);
            serverIP = response.ip;
        } else {
            console.log("response undefined");
            serverIP = "192.168.4.1";
        }

        serverURL = "http://" + serverURL;
        // console.log("FETCH - Server URL: " + serverURL);
        // console.log("FETCH - IP Address: " + serverIP);

        var path = window.location.pathname;
        page = path.split("/").pop();
        // console.log("page: " + page);

        if (page == "admin.htm") {

            document.forms['admin-form'].addEventListener('submit', (event) => {

                event.preventDefault();
                event.stopPropagation();

                if (!checkInputs()) {
                    return false;
                }

                dialog.render(1, "New Password Information", "Do you want to save the hew Admin Password?", warningColor, function (result) {
                    if (result === 0) {
                        return false;
                    }
                    // Just for testing on Mockoon - START //
                    if (serverURL.includes("localhost") || serverURL.includes("127.0.0.1")) {
                        // true - contains localhost or 127.0.0.1 add new serverURL;
                        document.forms['admin-form'].action = "http://127.0.0.1:3001/update";
                    }
                    // Just for testing on Mockoon - END //
                    // console.log(document.forms['global-form'].action);
                    // TODO do something here to show user that form is being submitted
                    fetch(event.target.action, {
                        method: 'POST',
                        body: new URLSearchParams(new FormData(event.target)) // event.target is the form
                    })
                        .then((resp) => {
                            if (!resp.ok) {
                                throw Error(resp.statusText + " - " + resp.url);
                            }
                            return resp.json();
                        })
                        .then((data) => {
                            if (data.result == 2) {
                                // Error occurred while uploading
                                dialog.render(0, "Error!", "Access denied!<br>You don't have permission to change the settings!", errorColor, function (result) { });
                            } else if (data.result == 1) {
                                // Upload finished successfully
                                dialog.render(0, "Success!", "New Admin password has been saved!", okColor, function (result) { });
                            } else {
                                // Warning occurred while uploading
                                dialog.render(0, "Warning!", "New Admin password has not been saved!", warningColor, function (result) { });
                            }
                        })
                        .catch(error => {
                            console.warn(error);
                            dialog.render(0, "Error!", error, errorColor, function (result) { });
                        });
                });
            });
        }
        // console.log("FETCH - END");
    });
})
/* Function to check all inputs */
function checkInputs() {
    /* Check admin input value */
    if (!checkValueLenght(document.getElementById("adm-pwd").value, 8)) {
        showError(document.getElementById("adm-pwd"));
        return false;
    }

    if (!checkValueLenght(document.getElementById("new-pwd-1").value, 8)) {
        showError(document.getElementById("new-pwd-1"));
        return false;
    }

    if (!checkValueLenght(document.getElementById("new-pwd-2").value, 8)) {
        showError(document.getElementById("new-pwd-2"));
        return false;
    }
    let str1 = document.getElementById("new-pwd-1").value;
    let str2 = document.getElementById("new-pwd-2").value;

    if (str1.localeCompare(str2) != 0) {
        showError(document.getElementById("new-pwd-1"));
        dialog.render(0, "Warning!", "Passwords do not match.", warningColor, function (result) { });
        return false;
    }

    return true;
}
/* Function to check value length of input */
function checkValueLenght(elementValue, minChar) {
    if (elementValue.length < minChar) {
        dialog.render(0, "Warning!", "Minimum " + minChar + " charachters.", warningColor, function (result) { });
        return false;
    }
    return true;
}
/* Function to shows error */
function showError(element) {
    element.style.background = "red";
    setTimeout(function () {
        element.style.background = "none";
    }, 5000);
}