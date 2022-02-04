/* Import main functions, variables from main.js */
import { warningColor, errorColor, okColor, scalePage, getvals, CustomAlertDialog } from '/resources/main.js';

/* Global variables */
var serverIP;   // IP address of server
var serverURL;  // URL of server
var page;
var hr;
var tbl;
var row;
var cell;

var Alert = new CustomAlertDialog();

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
            serverIP = response.ip;
        } else {
            console.log("response undefined");
            serverIP = "192.168.4.1";
        }

        serverURL = "http://" + serverURL;
        console.log("FETCH - Server URL: " + serverURL);
        console.log("FETCH - IP Address: " + serverIP);

        var path = window.location.pathname;
        page = path.split("/").pop();
        console.log("page: " + page);

        if (page == "" || page == "index.htm") {
            getWeatherData();
        }
        console.log("FETCH - END");
    });
})
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
            Alert.render("Error!", "Error occurred during getting weather data!", errorColor);
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
