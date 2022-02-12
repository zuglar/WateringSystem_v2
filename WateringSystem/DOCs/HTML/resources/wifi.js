/* Import main functions, variables from main.js */
import { warningColor, errorColor, okColor, scalePage, getvals, CustomDialog } from '/resources/main.js';

/* Global variables */
var serverIP;   // IP address of server
var serverURL;  // URL of server
var page;

var dialog = new CustomDialog();
/* Scale page to screen size */
$(function () {
    scalePage();
});

$(document).ready(function () {
    $('.navbar').load("./resources/navbar.htm");
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
        // document.getElementById("ip").textContent = "IP: " + serverIP;
        // document.getElementById("url").textContent = "URL: " + serverURL;
        // console.log("FETCH - Server URL: " + serverURL);
        // console.log("FETCH - IP Address: " + serverIP);

        var path = window.location.pathname;
        page = path.split("/").pop();
        // console.log("page: " + page);

        if (page == "wifi.htm") {
            showWifiPage();
        }
        // console.log("FETCH - END");
    });
})
/* Function to show wifi data from json object */
function showWifiPage() {
    var newServerURL = serverURL + "/getWiFi";
    // Just for testing on Mockoon - START //
    if (serverURL.includes("localhost") || serverURL.includes("127.0.0.1")) {
        // true - contains localhost or 127.0.0.1 add new serverURL
        newServerURL = 'http://127.0.0.1:3001/getWiFi';
    }
    // Just for testing on Mockoon - END //
    // get wifi json data
    getvals(newServerURL).then(response => {
        if (response !== undefined) {
            console.log("json wifi data loaded");
            // show wifi data on page
            document.getElementById("ap-ssid").value = response.wifi[0];
            document.getElementById("ap-ip").value = response.wifi[1];
            document.getElementById("sta-ip").value = response.wifi[2];
            document.getElementById("sta-subnet").value = response.wifi[3];
            document.getElementById("sta-gateway").value = response.wifi[4];
            document.getElementById("sta-dns").value = response.wifi[5];
            document.getElementById("sta-ssid").value = response.wifi[6];

            document.getElementById("server-url").value = serverURL;
            document.getElementById("server-ip").value = serverIP;

            setTooltip();
            document.getElementById('ap-save-chb').addEventListener('click', () => {
                apCheckBox(document.getElementById('ap-save-chb'));
                
            });

            document.getElementById('sta-static-chb').addEventListener('click', () => {
                staStaticCheckBox(document.getElementById('sta-static-chb'));
                
            });

            document.getElementById('sta-chb').addEventListener('click', () => {
                staCheckBox(document.getElementById('sta-chb'));
                
            });

            document.forms['wifi-form'].addEventListener('submit', (event) => {

                event.preventDefault();
                event.stopPropagation();
                let checkedSettings = wifiSettingsSave();
                if (!checkedSettings[0]) {
                    checkedSettings[1].style.background = "red";
                    setTimeout(function(){  
                        checkedSettings[1].style.background = "none";
                    }, 5000);
                   
                    return false;
                }

                dialog.render(1, "WiFi Setting Information", "Do you want to save the WiFi Setting?", warningColor, function (confirmed) {
                    if (confirmed === 0) {
                        return false;
                    }
                    // YES
                    // Just for testing on Mockoon - START //
                    if (serverURL.includes("localhost") || serverURL.includes("127.0.0.1")) {
                        // true - contains localhost or 127.0.0.1 add new serverURL;
                        document.forms['wifi-form'].action = "http://127.0.0.1:3001/update";
                    }
                    // Just for testing on Mockoon - END //

                    // const queryString = new URLSearchParams(new FormData(event.target)).toString();
                    // console.log(queryString);
                    // console.log(document.forms['wifi-form'].action);
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
                                dialog.render(0, "Error!", "Access denied!<br>You don't have permission to change the settings!", errorColor, function (result) {});
                            } else if (data.result == 1) {
                                // Upload finished successfully
                                dialog.render(0, "Success", "New WiFi setting has been saved!", okColor, function (result) {});
                            } else {
                                // Warning occurred while uploading
                                dialog.render(0, "WARNING!", "New WiFi setting has not been saved!", warningColor, function (result) {});
                            }
                        })
                        .catch(error => {
                            console.warn(error);
                            dialog.render(0, "Error!", error, errorColor, function (result) {});
                        });
                });
            });
        } else {
            dialog.render(0, "Error!", "Error occurred during getting wifi data!", errorColor, function (result) {});
        }
    });
}
/* Function to set tooltip to input elemets */
function setTooltip() {
    var ssid = document.getElementsByClassName("ssid");
    for (let i = 0; i < ssid.length; i++) {
        ssid[i].setAttribute("title", "Must be at least 4 characters.");
    }

    var pwd = document.getElementsByClassName("password");
    for (let i = 0; i < pwd.length; i++) {
        pwd[i].setAttribute("title", "Must be at least 8 characters.");
    }

    var pwd2 = document.getElementsByClassName("password2");
    for (let i = 0; i < pwd2.length; i++) {
        pwd2[i].setAttribute("title", "Must be the same as new password above.");
    }

    document.getElementsByClassName("ap-save")[0].title = "Enable if you want to set new name\nor password of System Access Point.";
    document.getElementsByClassName("sta-save")[0].title = "Using router- Enable the checkbox and fill in the data.\n" +
        "No router - Enable the checkbox and leave blank the SSID input.";
    document.getElementsByClassName("sta-static")[0].title = "Static IP - Enable the checkbox and fill in the data.\n" +
        "Dynamic IP - Enable the checkbox and leave blank the IP Address input.";

}
/* Function for AP checkbox */
function apCheckBox(checkbox) {
    if (checkbox.checked) {
        document.getElementById("ap-ssid").disabled = false;
        document.getElementById("ap-newpwd-1").disabled = false;
        document.getElementById("ap-newpwd-2").disabled = false;
    } else {
        document.getElementById("ap-ssid").disabled = true;
        document.getElementById("ap-newpwd-1").disabled = true;
        document.getElementById("ap-newpwd-2").disabled = true;
    }
    enableSave();
}
/* Function for STA checkbox */
function staCheckBox(checkbox) {
    if (checkbox.checked) {
        document.getElementById("sta-ssid").disabled = false;
        document.getElementById("sta-newpwd-1").disabled = false;
        document.getElementById("sta-newpwd-2").disabled = false;
    } else {
        document.getElementById("sta-ssid").disabled = true;
        document.getElementById("sta-newpwd-1").disabled = true;
        document.getElementById("sta-newpwd-2").disabled = true;
    }
    enableSave();
}
/* Function for STA STATIC checkbox */
function staStaticCheckBox(checkbox) {
    if (checkbox.checked) {
        document.getElementById("sta-ip").disabled = false;
        document.getElementById("sta-subnet").disabled = false;
        document.getElementById("sta-gateway").disabled = false;
        document.getElementById("sta-dns").disabled = false;
    } else {
        document.getElementById("sta-ip").disabled = true;
        document.getElementById("sta-subnet").disabled = true;
        document.getElementById("sta-gateway").disabled = true;
        document.getElementById("sta-dns").disabled = true;
    }
    enableSave();
}
/* Function to save new wifi settings */
function wifiSettingsSave() {
    /* Check Access Point input values */
    const apCheckBox = document.getElementById("ap-save-chb");
    if (apCheckBox.checked == true) {
        if (!checkValueLenght(document.getElementById("ap-ssid").value, 4)) {
            return [false, document.getElementById("ap-ssid")];
        }

        if (!checkValueLenght(document.getElementById("ap-newpwd-1").value, 8)) {
            return [false, document.getElementById("ap-newpwd-1")];
        }

        if (!compareStr(document.getElementById("ap-newpwd-1").value, document.getElementById("ap-newpwd-2").value)) {
            return [false, document.getElementById("ap-newpwd-1")];
        }
    }
    /* Check Satitic/Dynamic input values */
    const staStaticIpCheckBox = document.getElementById("sta-static-chb");
    if (staStaticIpCheckBox.checked == true) {
        if (document.getElementById("sta-ip").value.length !== 0) {
            if (!checkIPaddress(document.getElementById("sta-ip").value)) {
                return [false, document.getElementById("sta-ip")];
            }

            if (!checkIPaddress(document.getElementById("sta-subnet").value)) {
                return [false, document.getElementById("sta-subnet")];
            }

            if (!checkIPaddress(document.getElementById("sta-gateway").value)) {
                return [false, document.getElementById("sta-gateway")];
            }

            if (document.getElementById("sta-dns").value.length != 0) {
                if (!checkIPaddress(document.getElementById("sta-dns").value)) {
                    return [false, document.getElementById("sta-dns")];
                }
            }
        }
    }
    /* Check Station/Router input values */
    const staCheckBox = document.getElementById("sta-chb")
    if (staCheckBox.checked == true) {
        if (document.getElementById("sta-ssid").value.length != 0) {
            if (!checkValueLenght(document.getElementById("sta-ssid").value, 4)) {
                return [false, document.getElementById("sta-ssid")];
            }

            if (!checkValueLenght(document.getElementById("sta-newpwd-1").value, 8)) {
                return [false, document.getElementById("sta-newpwd-1")];
            }

            if (!compareStr(document.getElementById("sta-newpwd-1").value, document.getElementById("sta-newpwd-2").value)) {
                return [false, document.getElementById("sta-newpwd-1")];
            }
        }
    }

    if (apCheckBox.checked == true || staCheckBox.checked == true || staStaticIpCheckBox.checked == true) {
        /* Check admin input value */
        if (!checkValueLenght(document.getElementById("adm-pwd").value, 8)) {
            document.getElementById("adm-pwd").focus();
            return [false, document.getElementById("adm-pwd")];
        }

        return [true, 0];
    }
}
/* Function to enable or disbale admin password input and save button */
function enableSave() {
    if (document.getElementById("ap-save-chb").checked || document.getElementById("sta-chb").checked ||
        document.getElementById("sta-static-chb").checked) {
        document.getElementById("adm-pwd").disabled = false;
        document.getElementById("save-wifi-btn").disabled = false;
        return;
    }
    document.getElementById("adm-pwd").disabled = true;
    document.getElementById("save-wifi-btn").disabled = true;
    return;
}
/* Function to check value length of element by element id */
function checkValueLenght(elementValue, minChar) {
    if (elementValue.length < minChar) {
        dialog.render(0, "Warning!", "Minimum " + minChar + " charachters.", warningColor, function (result) {});
        return false;
    }
    return true;
}
/* Function to compare two strings */
function compareStr(str1, str2) {
    if (str1.localeCompare(str2) != 0) {
        dialog.render(0, "Warning!", "Passwords do not match.", warningColor, function (result) {});
        return false;
    }
    return true;
}
/* Function to validate format of IP addres */
function checkIPaddress(ipaddress) {
    if (/^(?!\.)((^|\.)([1-9]?\d|1\d\d|2(5[0-5]|[0-4]\d))){4}$/.test(ipaddress)) {
        return true;
    }
    dialog.render(0, "Warning!", "You have entered an invalid IP address.", warningColor, function (result) {});
    return false;
}
