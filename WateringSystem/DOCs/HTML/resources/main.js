/* Global variables */
var serverIP;   // IP address of server
var serverURL;  // URL of server
var page;
var hr;
var tbl;
var row;
var cell;
var warningColor = "#ffc107";
var errorColor = "#ff0000";
var okColor = "#698d19";
/* Scale page to screen size */
$(function () {
    /* Variables for main widow size and element size */
    var windowWidth = $(window).width();
    var mainDivWidth = document.getElementsByClassName("container").offsetWidth;

    if (windowWidth > mainDivWidth) {
        $(".container").css({
            left: ((windowWidth / 2) - (mainDivWidth / 2)) + "px"
        });
    }
    else {
        var scale = windowWidth / mainDivWidth;
        document.querySelector('meta[name="viewport"]').setAttribute('content', 'width=' + mainDivWidth + ', initial-scale=' + scale + '');
    }

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
        console.log("FETCH - Server URL: " + serverURL);
        console.log("FETCH - IP Address: " + serverIP);

        var path = window.location.pathname;
        page = path.split("/").pop();
        console.log("page: " + page);

        if (page == "" || page == "index.htm") {
            getWeatherData();
        } else if (page == "wifi.htm") {
            showWifiPage();
        }
        console.log("FETCH - END");
    });
})
/* Fetching the data */
function getvals(url) {
    return fetch(url,
        {
            method: "GET",
            headers: {
                'Accept': 'application/json',
                'Content-Type': 'application/json',
            },
            mode: "cors",
            cache: "default"
        })
        .then((response) => {
            // console.log(response);
            if (!response.ok) {
                throw Error(response.statusText + " - " + response.url);
            }
            return response.json();
        })
        .then((responseData) => {
            // console.log(responseData);
            return responseData;
        })
        .catch(error => console.warn(error));
}
/* Gets sensors data and valves status every 5 minutes  */
setInterval(() => {
    if (page == "" || page == "index.htm") {
        getWeatherData();
    }
}, 300000)
/* Function to send a request to get new values of sensors states */
function getWeatherData() {
    let newServerURL = serverURL + "/getWeather";
    // Just for testing on Mockoon - START //
    if (serverURL.includes("localhost") || serverURL.includes("127.0.0.1")) {
        // true - contains localhost or 127.0.0.1 add new serverURL
        newServerURL = 'http://127.0.0.1:3001/getWeather';
    }
    // Just for testing on Mockoon - END //
    // console.log("getWeatherData() - newServerURL : " + newServerURL);
    // get weather json data
    getvals(newServerURL).then(response => {
        if (response !== undefined) {
            console.log("json weather data loaded");
            // console.log("getWeatherData() - data: " + response + "\n" + response.temp + "\n" + response.hum + "\n" + response.atm);
            // temperature value (JSON string)
            document.getElementById("temp-td").textContent = response.temp;
            // humidity value (JSON string)
            document.getElementById("hum-td").textContent = response.hum;
            // Atmospheric pressure value (JSON string)
            document.getElementById("atm-td").textContent = response.atm;
            // "table-div" is dynamic element
            // checks if is exists we have to delete it first and then creates it
            if (document.getElementById("sensors-table-div")) {
                $("#sensors-table-div").empty();
            }
            // response.valves - contains state of valves (JSON decimal number)
            valvesState(response.valves);
            // response.sensors - percentage of soil wetness per sensors (JSON string)
            wetnessRainSensorsState(response.sensors);
        } else {
            // alert("Error occurred during getting weather data! !!! ");
            showAlert("Error occurred during getting weather data!", errorColor);
        }
    });
}
/* Decimal to binary */
function dec2bin(dec) {
    let bin = (dec >>> 0).toString(2);
    if (bin.length < 8) {
        for (let i = bin.length; i < 8; i++) {
            bin = "0" + bin;
        }
    }
    return bin;
}
/* Function to show state of valves */
function valvesState(valvesDecimal) {
    // console.log("valvesState - typeof(valvesDecimal): " + typeof (valvesDecimal) + ", value: " + valvesDecimal);
    const valves = dec2bin(valvesDecimal);

    if (valves.length != 8) {
        alert("\tERROR!!!\nUnable to get data of state of valves!");
        return;
    }
    // get the reference for the div
    const tableDiv = document.getElementById("sensors-table-div");
    // creates a <hr> element
    hr = document.createElement("hr");
    // sets class of <hr> element
    hr.setAttribute("class", "hr");
    // appends <hr> into <div>
    tableDiv.appendChild(hr);
    // creates a <table> element
    tbl = document.createElement("table");
    tbl.id = "valves-table";
    // sets class of <table> element
    tbl.classList.add("content-table");
    // sets style width of <table> element
    tbl.style.width = "85%";
    // creates a <tr> element
    row = document.createElement("tr");
    // sets class of <tr> element
    row.classList.add("info-tr");
    // creates a <td> element
    cell = document.createElement("td");
    // sets attribute of <td> element
    cell.setAttribute("colspan", "8");
    // create text node and appends into <td>
    cell.appendChild(document.createTextNode("Valves State"));
    // appends <td> into <tr>
    row.appendChild(cell);
    // appends <tr> into <table>
    tbl.appendChild(row);
    // appends <table> into <div>
    tableDiv.appendChild(tbl);

    let numOfRows = 2;
    for (var i = 0; i < numOfRows; i++) {
        // creates a <tr> element
        row = document.createElement("tr");
        for (var j = valves.length - 1; j >= 0; j--) {
            // creates a <td> element
            cell = document.createElement("td");
            if (i == 0) {
                // create text node and appends into <td>
                cell.appendChild(document.createTextNode((8 - j) + "."));
            } else if (i == 1) {
                // creates a <img> element
                var img = document.createElement("img");
                if (valves[j] == "0") {
                    img.src = "./icons/switch-off.png";
                } else {
                    img.src = "./icons/switch-on.png";
                }
                // appends <img> into <td>
                cell.appendChild(img);
            }
            // appends <td> into <tr>
            row.appendChild(cell);
        }
        // appends <tr> into <table>
        tbl.appendChild(row);
    }
    // appends <table> into <div>
    tableDiv.appendChild(tbl);
}
/* Function to show state of wetness of soil and sensors of rain */
function wetnessRainSensorsState(sensors) {
    // var sensors = document.getElementById("wetness-rain").textContent;
    // console.log("wetnessRainSensorsState - typeof(sensors): " + typeof (sensors) + ", value: " + sensors);
    const valueArray = sensors.split(';');
    if (valueArray.length != 9) {
        // alert("ERROR!!!\nUnable to read wetness and rain sensors values!!!");
        showAlert("ERROR! Unable to read wetness and rain sensors values!", errorColor);
        return;
    }
    // get the reference for the div
    const tableDiv = document.getElementById("sensors-table-div");
    // creates a <hr> element
    hr = document.createElement("hr");
    // sets class of <hr> element
    hr.setAttribute("class", "hr");
    // appends <hr> into <div>
    tableDiv.appendChild(hr);
    // creates a <table> element
    tbl = document.createElement("table");
    // sets class of <table> element
    tbl.classList.add("content-table");
    // sets style width of <table> element
    tbl.style.width = "85%";
    // creates a <tr> element
    row = document.createElement("tr");
    // sets class of <tr> element
    row.classList.add("info-tr");
    // creates a <td> element
    cell = document.createElement("td");
    // sets attribute of <td> element
    cell.setAttribute("colspan", "8");
    // create text node and appends into <td>
    cell.appendChild(document.createTextNode("Soil Wetness State"));
    // appends <td> into <tr>
    row.appendChild(cell);
    // appends <tr> into <table>
    tbl.appendChild(row);
    // appends <table> into <div>
    tableDiv.appendChild(tbl);
    var wetnessSensorNumber = 8;

    let numOfRows = 2;
    for (var i = 0; i < numOfRows; i++) {
        // creates a <tr> element
        row = document.createElement("tr");
        for (var j = 0; j < wetnessSensorNumber; j++) {
            // creates a <td> element
            cell = document.createElement("td");
            if (i == 0) {
                // create text node and appends into <td>
                cell.appendChild(document.createTextNode((j + 1) + "."));
            } else if (i == 1) {
                cell.appendChild(document.createTextNode(valueArray[j] + "%"));
            }
            // appends <td> into <tr>
            row.appendChild(cell);
        }
        // appends <td> into <tr>
        tbl.appendChild(row);
    }
    // appends <table> into <div>
    tableDiv.appendChild(tbl);

    hr = document.createElement("hr");
    // sets class of <hr> element
    hr.setAttribute("class", "hr");
    // appends <hr> into <div>
    tableDiv.appendChild(hr);

    // creates a <table> element
    tbl = document.createElement("table");
    // sets class of <table> element
    tbl.classList.add("content-table");
    // creates a <tr> element
    row = document.createElement("tr");
    // sets class of <tr> element
    row.classList.add("info-tr");
    // creates a <td> element
    cell = document.createElement("td");
    // sets attribute of <td> element
    cell.setAttribute("colspan", "4");
    // create text node and appends into <td>
    cell.appendChild(document.createTextNode("Rain Senor State"));
    // appends <td> into <tr>
    row.appendChild(cell);
    // appends <tr> into <table>
    tbl.appendChild(row);
    // appends <table> into <div>
    tableDiv.appendChild(tbl);
    // creates a <tr> element
    row = document.createElement("tr");
    // appends <table> into <div>
    tableDiv.appendChild(tbl);
    // creates a <td> element
    cell = document.createElement("td");
    // creates a <img> element
    var img = document.createElement("img");
    if (valueArray[8] == 0) {
        img.src = "./icons/sun.png";
    } else {
        img.src = "./icons/rain.png";
    }
    // appends <img> into <td>
    cell.appendChild(img);
    // appends <td> into <tr>
    row.appendChild(cell);
    // appends <td> into <tr>
    tbl.appendChild(row);
}
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

            document.forms['wifi-form'].addEventListener('submit', (event) => {
                if (!wifiSettingsSave()) {
                    event.preventDefault();
                    event.stopPropagation();
                    return false;
                }
                // Just for testing on Mockoon - START //
                if (serverURL.includes("localhost") || serverURL.includes("127.0.0.1")) {
                    // true - contains localhost or 127.0.0.1 add new serverURL;
                    document.forms['wifi-form'].action = "http://127.0.0.1:3001/upload";
                }
                // Just for testing on Mockoon - END //

                // stop form from submitting normally
                event.preventDefault();

                const queryString = new URLSearchParams(new FormData(event.target)).toString();
                console.log(queryString);
                console.log(document.forms['wifi-form'].action);
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
                            showAlert("Access denied!\nYou don't have permission to change the settings!", errorColor);
                        } else if (data.result == 1) {
                            // Upload finished successfully
                            showAlert("New WiFi setting has been saved!", okColor);
                        } else {
                            // Warning occurred while uploading
                            showAlert("New WiFi setting has not been saved!", warningColor);
                        }
                    })
                    .catch(error => {
                        console.warn(error);
                        showAlert(error, errorColor);
                    });
            });
        } else {
            // alert("Error occurred during getting wifi data! !!! ");
            showAlert("Error occurred during getting wifi data! !!!", errorColor);
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
    /* Check Satitic/Dynamic input values */
    const staStaticIpCheckBox = document.getElementById("sta-static-chb");
    if (staStaticIpCheckBox.checked == true) {
        if (document.getElementById("sta-ip").value.length !== 0) {
            if (!checkIPaddress(document.getElementById("sta-ip").value)) {
                document.getElementById("sta-ip").focus();
                return false;
            }

            if (!checkIPaddress(document.getElementById("sta-subnet").value)) {
                document.getElementById("sta-subnet").focus();
                return false;
            }

            if (!checkIPaddress(document.getElementById("sta-gateway").value)) {
                document.getElementById("sta-gateway").focus();
                return false;
            }

            if (document.getElementById("sta-dns").value.length != 0) {
                if (!checkIPaddress(document.getElementById("sta-dns").value)) {
                    document.getElementById("sta-dns").focus();
                    return false;
                }
            }
        }
    }
    /* Check Station/Router input values */
    const staCheckBox = document.getElementById("sta-chb")
    if (staCheckBox.checked == true) {
        if (document.getElementById("sta-ssid").value.length != 0) {
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
    }

    if (apCheckBox.checked == true || staCheckBox.checked == true || staStaticIpCheckBox.checked == true) {
        /* Check admin input value */
        if (!checkValueLenght(document.getElementById("adm-pwd").value, 8)) {
            document.getElementById("adm-pwd").focus();
            return false;
        }
        /* Alert to confirm to save the new values */
        if (!window.confirm("Are you sure you want to save new settings?")) {
            return false;
        }
        return true;
    }
    return false;
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
        // alert("\t\t\tERROR !\nMinimum " + minChar + " charachters.");
        showAlert("Warninig! Minimum " + minChar + " charachters. Warning focused.", warningColor);
        return false;
    }
    return true;
}
/* Function to compare two strings */
function compareStr(str1, str2) {
    if (str1.localeCompare(str2) != 0) {
        // alert("\t\t\tERROR !\nPasswords do not match.");
        showAlert("Warninig! Passwords do not match. Warning focused.", warningColor);
        return false;
    }
    return true;
}
/* Function to validate format of IP addres */
function checkIPaddress(ipaddress) {
    if (/^(?!\.)((^|\.)([1-9]?\d|1\d\d|2(5[0-5]|[0-4]\d))){4}$/.test(ipaddress)) {
        return true;
    }
    // alert("\t\t\tERROR!!!\nYou have entered an invalid IP address!")
    showAlert("Warninig! You have entered an invalid IP address. Warning focused.", warningColor);
    return false;
}
/* Function to show alert message */
function showAlert(message, color) {
    // Checks if is exists we have to delete it first and then creates it
    if (document.getElementById("show-message")) {
        $("#show-message").empty();
    }

    // Create paragraph, set attributes and text
    var p = document.createElement("p");
    p.setAttribute("id", "show-message");
    p.setAttribute("style", "opacity:0.85; -moz-opacity:0.85; filter:alpha(opacity=85); border-radius: 5px; width: 440px;" +
        "height: 50px; display: flex; justify-content: center; align-content: center; flex-direction: column; text-align: center;");
    var text = document.createTextNode(message);
    p.style.backgroundColor = color;
    p.appendChild(text);
    // Calculating horizontal position - margin left style
    if (parseInt($(window).width()) > parseInt(p.style.width)) {
        let margLeft = ((parseInt($(window).width()) - parseInt(p.style.width)) / 2) + "px";
        console.log("margLeft: " + margLeft);
        document.getElementById("save-result").style.marginLeft = margLeft;
    }
    // Calculating vertical position - top style
    if (parseInt($(window).height()) > parseInt(p.style.height)) {
        let posTop = (parseInt(($(window).height()) - parseInt(p.style.height)) / 2);
        posTop = (Math.round((posTop / parseInt($(window).height())) * 100)) + "%";
        // console.log("posTop: " + posTop);
        document.getElementById("save-result").style.top = posTop;
    }
    document.getElementById("save-result").appendChild(p);
    document.getElementById("save-result").style.visibility = "visible";
    document.getElementById("save-wifi-btn").disabled = true;
    // Shows message for 2 sec
    var count = 2;
    var time = setInterval(function () {
        count--;

        if (count == 0) {
            document.getElementById("save-result").removeChild(p);
            document.getElementById("save-result").style.visibility = "hidden";
            document.getElementById("save-wifi-btn").disabled = false;
        }
    }, 1000);
}
