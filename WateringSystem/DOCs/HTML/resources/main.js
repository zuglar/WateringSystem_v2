/* Global Variables */
var page;
var hr;
var tbl;
var row;
var cell;
var serverIP;
var serverURL;

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

    fetch('https://api.ipify.org/?format=json')
        .then(response => response.json())
        .then((ipAddress) => {
            serverIP = ipAddress.ip;
            console.log("Server IP Address:" + serverIP);

            var path = window.location.pathname;
            page = path.split("/").pop();
            console.log("page: " + page);
            
            // console.log("json: host: " + $(location).attr('host'));
    	    // console.log("json: hostname: " + $(location).attr('hostname'));
            // console.log("json: protocol: " + $(location).attr('protocol'));

            serverURL = $(location).attr('hostname');

            console.log("1. serverURL: " + serverURL);
            // loaclhost is just for testing
            if (serverURL.indexOf("localhost") === 0 || serverURL.indexOf("127.0.0.1") === 0) {
                // true - contains localhost or 127.0.0.1 add new serverURL
                serverURL = 'localhost:3001';
            }
            // console.log("2. serverURL: " + serverURL);
            if (serverURL.indexOf("https") === 0) {
                // true - contains https and replace it to http
                serverURL = serverURL.replace("https", "http");
            }
            // console.log("3. serverURL: " + serverURL);
            if(serverURL.indexOf("http://") !== 0) {
                // true - not contain http:// we add it
                serverURL = "http://" + serverURL;
            }
            console.log("4. serverURL: " + serverURL);
            if (page == "" || page == "index.htm") {
                //serverURL +="/getWeather";
                getWeatherData();

            } else if (page == "wifi.htm") {
                //serverURL +="/getWiFi";
                wifiSettingsShow();
            }
        })
});
/* Refresh sensors data and valves status every 1 minutes  */
setInterval(() => {
    if (page == "" || page == "index.htm") {
        getWeatherData();
    }
}, 60000);
/* Function to send a request to get new values of state of sensors. */
function getWeatherData() {
    console.log("getWeatherData() - serverURL: " + serverURL);
    let getWeatherDataURL = serverURL + '/getWeather';
    console.log("getWeatherData() - serverURL: " + serverURL);
    $.getJSON(getWeatherDataURL, function (data) {
        
    }).done(function (data) {
        console.log("json weather data loaded");
        console.log("getWeatherData() - data: " + data + "\n" + data.temp + "\n" + data.hum + "\n" + data.atm);
        // temperature value (JSON string)
        document.getElementById("temp-td").textContent = data.temp;
        // humidity value (JSON string)
        document.getElementById("hum-td").textContent = data.hum;
        // Atmospheric pressure value (JSON string)
        document.getElementById("atm-td").textContent = data.atm;
        // "table-div" is dynamic element
        // checks if is exists we have to delete it first and then creates it
        if (document.getElementById("table-div"))
            $("#table-div").empty();
        // Value which contains state of valve (JSON decimal number)
        valvesState(data.valves);
        // Value of percentage of wetness of soil per sensors (JSON string)
        wetnessRainSensorsState(data.sensors);
    }).fail(function() {
        alert("Error occurred !!! ");
        window.location.href = "./resources/notfound.htm";
    })
    /*         
            fetch(server)
                .then(res => res.json())
                .then((out) => {
    
                    console.log('Output: ', out);
                    document.getElementById("temp-td").textContent = out.temp;
                    document.getElementById("hum-td").textContent = out.hum;
                    document.getElementById("atm-td").textContent = out.atm;
                    // document.getElementById("valves").innerHTML= out.valves;
                    // valves = document.getElementById("valves").textContent = out.valves;   
                    // console.log("---valves: " + valves);
                
                }).catch(err => console.error(err));
     */
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
/* Function to show states of valves */
function valvesState(valvesDecimal) {
    console.log("valvesState - typeof(valvesDecimal): "  + typeof(valvesDecimal) + ", value: " + valvesDecimal);
    const valves = dec2bin(valvesDecimal);

    if (valves.length != 8) {
        alert("\tERROR!!!\nUnable to get data of state of valves!");
        return;
    }
    // get the reference for the div
    const tableDiv = document.getElementById("table-div");
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
                    img.src = "./icns/switch-off.png";
                } else {
                    img.src = "./icns/switch-on.png";
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
    console.log("wetnessRainSensorsState - typeof(sensors): "  + typeof(sensors) + ", value: " + sensors);
    const valueArray = sensors.split(';');
    if (valueArray.length != 9) {
        alert("ERROR!!!\nUnable to read wetness and rain sensors values!!!");
        return;
    }
    // get the reference for the div
    const tableDiv = document.getElementById("table-div");
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

    /* for (var i = 0; i < numOfRows; i++)  {
        // creates a <tr> element
        row = document.createElement("tr");
        for (var j = wetnessSensorNumber; j < valueArray.length; j++) {
            // creates a <td> element
            cell = document.createElement("td");
            if (i == 0) {
                // create text node and appends into <td>
                cell.appendChild(document.createTextNode((j -7) + "."));
            } else if (i == 1) {
                // creates a <img> element
                var img = document.createElement("img");
                if(valueArray[j] == 0) {
                    img.src = "./icns/switch-off.png";
                } else {
                    img.src = "./icns/rain.png";
                }
                // appends <img> into <td>
                cell.appendChild(img);
            }
            // appends <td> into <tr>
            row.appendChild(cell);
        }
        // appends <td> into <tr>
        tbl.appendChild(row);
    } */
    // creates a <tr> element
    row = document.createElement("tr");

    // appends <table> into <div>
    tableDiv.appendChild(tbl);
    // creates a <td> element
    cell = document.createElement("td");
    // creates a <img> element
    var img = document.createElement("img");
    if (valueArray[8] == 0) {
        img.src = "./icns/sun.png";
    } else {
        img.src = "./icns/rain.png";
    }
    // appends <img> into <td>
    cell.appendChild(img);
    // appends <td> into <tr>
    row.appendChild(cell);
    // appends <td> into <tr>
    tbl.appendChild(row);
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
            /* Check ip addreses input values */
            if (staStaticIpCheckBox.checked == true) {
                let staticIP = false;
                let ip = 0;
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
        } else {
            alert("\t\tWARNING!!!\nYou have not filled in input of Station/Route of SSID.\nThe Station/Router will not be configured.");
        }
    }

    if (apCheckBox.checked == true || staCheckBox.checked == true) {
        /* Check admin input value */
        if (!checkValueLenght(document.getElementById("adm-pwd").value, 8)) {
            document.getElementById("adm-pwd").focus();
            return false;
        }
        /* Alert to confirm to save new values */
        if (!window.confirm("Are you sure you want to save new settings?")) {
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
    document.getElementById("server-url").value = serverURL;
    document.getElementById("server-ip").value = serverIP;
    console.log("WIFI serverURL: " + serverURL);
    serverURL +="/getWiFi";
    console.log("WIFI serverURL: " + serverURL);
    $.getJSON(serverURL, function (data) {

    }).done(function (data){
        // Solution for error in browser:
        // Uncaught SyntaxError: JSON.parse: unexpected character at line 1 column 2 of the JSON data
        // START CODE
        let parsedData;
        try {
            console.log(JSON.stringify(data));
            parsedData = JSON.parse(JSON.stringify(data));
          } catch (err) {
            // Uncaught SyntaxError: JSON.parse: unexpected character at
            // line 1 column 2 of the JSON data
            console.log(err.message);
          }
        // END CODE  
        if (!data.hasOwnProperty("wifi")) {
            alert("\tERROR!!!\nUnable to read WiFi settings data!!!");
            return;
        }
        console.log(parsedData.wifi);
        console.log("json wifi data loaded");
        document.getElementById("ap-ssid").value = parsedData.wifi[0];
        document.getElementById("ap-ip").value = parsedData.wifi[1];
        document.getElementById("sta-ssid").value = parsedData.wifi[2];
        document.getElementById("sta-ip").value = parsedData.wifi[3];
        document.getElementById("sta-subnet").value = parsedData.wifi[4];
        document.getElementById("sta-gateway").value = parsedData.wifi[5];
        document.getElementById("sta-dns").value = parsedData.wifi[6];
    }).fail(function() {
        alert("Error occurred !!! ");
        window.location.href = "./resources/notfound.htm";
    })
}
