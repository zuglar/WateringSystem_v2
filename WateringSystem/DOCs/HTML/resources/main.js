/* Variables */
var page = "";
var ip = 0;

$(function () {
    /* Variables for main widow size and element size */
    /* var windowWidth = $(window).width();
    var mainDivWidth = document.getElementById("container").offsetWidth;
 
    if (windowWidth > mainDivWidth) {
        $("#container").css({
            left: ((windowWidth / 2) - (mainDivWidth / 2)) + "px"
        });
    }
    else {
        var scale = windowWidth / mainDivWidth;
        document.querySelector('meta[name="viewport"]').setAttribute('content', 'width=' + mainDivWidth + ', initial-scale=' + scale + '');
    }
*/
    /* var path = window.location.pathname;
    page = path.split("/").pop();
    console.log(page);

    if (page == "" || page == "index.htm") {
        valvesState();
        wetnessRainSensorsState();
    }*/
});
/* Load navbar from htm file */
$(document).ready(function () {
    $('.navbar').load("./resources/navbar.htm");

    var path = window.location.pathname;
    page = path.split("/").pop();
    console.log("page: " + page);

    if (page == "" || page == "index.htm") {
        valvesState();
        wetnessRainSensorsState();
    } else if (page == "wifi.htm") {
        wifiSettingsShow();
    }
});
/* Function to show states of valves */
function valvesState() {
    var valves = document.getElementById("valves").textContent;
    console.log("valves: " + valves);
    if (valves.length != 8) {
        alert("\tERROR!!!\nUnable to get data of state of valves!");
    }

    var row = document.getElementById("valves-state");
    for (var i = valves.length - 1; i >= 0; i--) {
        var img = document.createElement('img');
        var ttd = document.createElement("td");

        if (valves[i] == "0") {
            img.src = "./icns/green.png";
        } else {
            img.src = "./icns/red.png";
        }
        ttd.appendChild(img);
        row.appendChild(ttd);
    }
}
/* Function to show state of wetness of soil and sensors of rain */
function wetnessRainSensorsState() {
    var sensors = document.getElementById("wetness-rain").textContent;
    console.log("sensors: " + sensors)
    const valueArray = sensors.split(';', 10);
    if (valueArray.length != 10) {
        alert("ERROR!!!\nUnable to read wetness and rain sensors values!!!");
        return;
    }

    var wetnessSensorNumber = 8;

    var wetness = document.getElementById("wetness-state");
    var rain = document.getElementById("rain-state");

    for (let i = 0; i < valueArray.length; i++) {
        var ttd = document.createElement("td");

        if (i < wetnessSensorNumber) {
            ttd.innerHTML = valueArray[i] + "%";
            wetness.appendChild(ttd);
        } else {
            var img = document.createElement('img');
            if (valueArray[i] == 0) {
                img.src = "./icns/green.png";
            } else {
                img.src = "./icns/red.png";
            }
            ttd.appendChild(img);
            rain.appendChild(ttd);
        }
    }
}
/* Refresh sensors data and valves status every 5 minutes  */
setInterval(() => {
    if (page == "" || page == "index.htm") {
        getData();
    }
}, 300000);
/* Function to send request to get new values of sensors. */
function getData() {
    var xhr = new XMLHttpRequest();
    console.log("getData() - page: " + page);
    if (page == "" || page == "index.htm") {
        xhr.open("GET", "/update?update=1", true);
        xhr.send();
        setTimeout("location.reload(true);", 5000);
    }
}
/* Function for saving new wifi settings */
function wifiSettingsSave() {
    const apCheckBox = document.getElementById("ap-save-chb");
    const staCheckBox = document.getElementById("sta-save-chb")
    const staStaticIpCheckBox = document.getElementById("sta-static-chb");
    /* Check Access Point input values */
    if (apCheckBox.checked == true) {
        if (!checkValueLenght(document.getElementById("ap-ssid").value, 4)) {
            document.getElementById("ap-ssid").focus();
            return false;
        }

        if (!checkValueLenght(document.getElementById("ap-newpwd-1").value, 8)) {
            document.getElementById("ap-newpwd-1").focus();
            return false;
        }

        if (!compareStr(document.getElementById("ap-newpwd-1").value, document.getElementById("ap-newpwd-2").value)) {
            document.getElementById("ap-newpwd-1").focus();
            return false;
        }
    }
    /* Check Station input values */
    if (staCheckBox.checked == true) {
        if (!checkValueLenght(document.getElementById("sta-ssid").value, 4)) {
            document.getElementById("sta-ssid").focus();
            return false;
        }

        if (!checkValueLenght(document.getElementById("sta-newpwd-1").value, 8)) {
            document.getElementById("sta-newpwd-1").focus();
            return false;
        }

        if (!compareStr(document.getElementById("sta-newpwd-1").value, document.getElementById("sta-newpwd-2").value)) {
            document.getElementById("sta-newpwd-1").focus();
            return false;
        }
    }
    /* Check ip addreses input values */
    if (staStaticIpCheckBox.checked == true) {
        let staticIP = false;
        ip = document.getElementById("sta-ip").value;
        if (ip.length != 0) {
            if (!checkIPaddress(ip)) {
                document.getElementById("sta-ip").focus();
                return false;
            }
            staticIP = true;
        }

        if (staticIP) {
            ip = document.getElementById("sta-subnet").value;
            if (!checkIPaddress(ip)) {
                document.getElementById("sta-subnet").focus();
                return false;
            }

            ip = document.getElementById("sta-gateway").value;
            if (!checkIPaddress(ip)) {
                document.getElementById("sta-gateway").focus();
                return false;
            }

            ip = document.getElementById("sta-dns").value;
            if (ip.length != 0) {
                if (!checkIPaddress(ip)) {
                    document.getElementById("sta-dns").focus();
                    return false;
                }
            }
        }
    }

    if (apCheckBox.checked == true || staCheckBox.checked == true) {
        /* Check admin input value */
        if (!checkValueLenght("adm-pwd"))
            return false;
        /* Alert to confirm to save new values */
        if (!window.confirm("Are you sure you want to save the new settings?")) {
            alert("The new settings has not been saved!");
            return false;
        }
        return true;
    }
    return false;
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
        document.getElementById("sta-static-chb").disabled = false;
        document.getElementById("sta-ip").disabled = false;
        document.getElementById("sta-subnet").disabled = false;
        document.getElementById("sta-gateway").disabled = false;
        document.getElementById("sta-dns").disabled = false;
    } else {
        document.getElementById("sta-ssid").disabled = true;
        document.getElementById("sta-newpwd-1").disabled = true;
        document.getElementById("sta-newpwd-2").disabled = true;
        document.getElementById("sta-static-chb").disabled = true;
        document.getElementById("sta-ip").disabled = true;
        document.getElementById("sta-subnet").disabled = true;
        document.getElementById("sta-gateway").disabled = true;
        document.getElementById("sta-dns").disabled = true;
    }
    enableSave();
}
/* Function to enable or disbale admin password input and save button */
function enableSave() {
    if (document.getElementById("ap-save-chb").checked || document.getElementById("sta-save-chb").checked) {
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
        alert("\tERROR !\nMinimum " + minChar + " charachters.");
        return false;
    }
    return true;
}
/* Function to compare two strings */
function compareStr(str1, str2) {
    if (str1.localeCompare(str2) != 0) {
        alert("\tERROR !\nPasswords do not match ");
        return false;
    }
    return true;
}
/* Function to validate format of IP addres */
function checkIPaddress(ipaddress) {
    if (/^(?!\.)((^|\.)([1-9]?\d|1\d\d|2(5[0-5]|[0-4]\d))){4}$/.test(ipaddress)) {
        return true;
    }
    alert("\tERROR!!!\nYou have entered an invalid IP address!")
    return false;
}
/* Function to show WiFi setting values */
function wifiSettingsShow() {
    var data = document.getElementById("wifi-data").textContent;
    console.log("wifi data: " + data)
    const valueArray = data.split(';', 7);
    if (valueArray.length != 7) {
        alert("\tERROR!!!\nUnable to read WiFi settings data!!!");
        return;
    }
    document.getElementById("ap-ssid").value = valueArray[0];
    document.getElementById("ap-ip").value = valueArray[1];
    document.getElementById("sta-ssid").value = valueArray[2];
    document.getElementById("sta-ip").value = valueArray[3];
    document.getElementById("sta-subnet").value = valueArray[4];
    document.getElementById("sta-gateway").value = valueArray[5];
    document.getElementById("sta-dns").value = valueArray[6];
}
