/* Import main functions, variables from main.js */
import { warningColor, errorColor, okColor, scalePage, getvals, CustomAlertDialog, CustomConfirmDialog } from '/resources/main.js';

/* Global variables */
var serverIP;   // IP address of server
var serverURL;  // URL of server
var newServerURL;
var page;
var selectedRuleItemText;
var tbl;
var row;
var cell;
var input;
const VALVES_NUMBER = 8;

var Alert = new CustomAlertDialog();
var confirm = new CustomConfirmDialog();

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
        console.log("FETCH - Server URL: " + serverURL);
        console.log("FETCH - IP Address: " + serverIP);

        var path = window.location.pathname;
        page = path.split("/").pop();
        console.log("page: " + page);

        if (page == "rules.htm") {
            console.log("page: rules.htm");

            addValvesTable();
            createThresholdsInput();
            getSetGlobalData();
        }
        console.log("FETCH - END");
    });
})
/* Function to get, set Global data and add name of rule to select element */
function getSetGlobalData() {
    newServerURL = serverURL + "/getGlobalData";
    // Just for testing on Mockoon - START //
    if (serverURL.includes("localhost") || serverURL.includes("127.0.0.1")) {
        // true - contains localhost or 127.0.0.1 add new serverURL
        newServerURL = 'http://127.0.0.1:3001/getGlobalData';
    }
    // Just for testing on Mockoon - END //
    // get rule name from json global data data
    getvals(newServerURL).then(response => {
        if (response !== undefined) {
            console.log("json global data loaded");
            const rulesSelect = document.getElementById("rules-select");
            const ruleNames = response.rulename;
            for (var i = 0; i < ruleNames.length; i++) {
                var opt = document.createElement("option");
                opt.innerHTML = ruleNames[i];
                opt.value = i;
                opt.id = "select-option-" + i;
                rulesSelect.add(opt);
            }
            rulesSelect.addEventListener('change', (event) => {
                selectedRule();
            });

            document.getElementById("temp-sensor-enabled").addEventListener('change', (event) => {
                tempCheckBox(document.getElementById("temp-sensor-enabled"));
            });
            // Save Global settings
            document.forms['global-form'].addEventListener('submit', (event) => {

                event.preventDefault();
                event.stopPropagation();
                if (!checkGlobalSettings()) {
                    return false;
                }

                confirm.render(1, "Global Setting Information", "Do you want to save the Global Setting?", warningColor, function (result) {
                    console.log("......result: " + result);
                    if (result === 0) {
                        return false;
                    }

                    // Just for testing on Mockoon - START //
                    if (serverURL.includes("localhost") || serverURL.includes("127.0.0.1")) {
                        // true - contains localhost or 127.0.0.1 add new serverURL;
                        document.forms['global-form'].action = "http://127.0.0.1:3001/update";
                    }
                    // Just for testing on Mockoon - END //
                    console.log(document.forms['global-form'].action);
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
                                Alert.render("Error!", "Access denied!<br>You don't have permission to change the settings!", errorColor);
                            } else if (data.result == 1) {
                                // Upload finished successfully
                                Alert.render("Success!", "New Global setting has been saved!", okColor);
                            } else {
                                // Warning occurred while uploading
                                Alert.render("Warning!", "New Global setting has not been saved!", warningColor);
                            }
                        })
                        .catch(error => {
                            console.warn(error);
                            Alert.render("Error!", error, errorColor);
                        });
                });


            });
            // Save or Delete rule settings
            document.forms['rule-form'].addEventListener('submit', (event) => {

                event.preventDefault();
                event.stopPropagation();
                if (!checkRuleData()) {
                    return false;
                }

                confirm.render(2, "Rule Setting Information", "Do you want to save or delete the rule?", warningColor, function (result) {
                    if (result === 0) {
                        // DELETE RULE
                        document.getElementById("save-delete").value = "0";
                    } else if (result === 1) {
                        // SAVE RULE
                        document.getElementById("save-delete").value = "1";
                    } else if (result === 2) {
                        // CANCEL
                        return false;
                    }

                    // Just for testing on Mockoon - START //
                    if (serverURL.includes("localhost") || serverURL.includes("127.0.0.1")) {
                        // true - contains localhost or 127.0.0.1 add new serverURL;
                        document.forms['rule-form'].action = "http://127.0.0.1:3001/upload";
                    }
                    // Just for testing on Mockoon - END //
                    console.log(document.forms['rule-form'].action);
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
                                Alert.render("Error!", "Access denied!<br>You don't have permission to change the settings!", errorColor);
                            } else if (data.result == 1) {
                                // Upload finished successfully
                                Alert.render("Success!", "Rule setting has been saved / deleted!", okColor);
                            } else {
                                // Warning occurred while uploading
                                Alert.render("Warning!", "Rule setting has not been saved / deleted!", warningColor);
                            }
                        })
                        .catch(error => {
                            console.warn(error);
                            Alert.render("Error!", error, errorColor);
                        });
                });
            });
            setGlobalData(response);
            selectedRule();
        } else {
            Alert.render("Error!", "Error occurred during getting Global Data!", errorColor);
        }
    });
}
/*  Function to get data of selected rule */
function selectedRule() {
    // Gets option value from select element
    var selectedItemIndex = document.getElementById("rules-select").value;
    // console.log("selectedItemIndex: " + selectedItemIndex);
    // Gets option text from select element
    selectedRuleItemText = document.getElementById("rules-select").options[selectedItemIndex].text;
    // console.log("selectedRuleItemText: " + selectedRuleItemText);
    // // Set rule name to input elemet 
    // // https://stackoverflow.com/questions/1085801/get-selected-value-in-dropdown-list-using-javascript?rq=1
    document.getElementById("rule-name-input").value = selectedRuleItemText;
    newServerURL = serverURL + '/getRuleValue?ruleName=' + selectedRuleItemText;
    // Just for testing on Mockoon - START //
    if (serverURL.includes("localhost") || serverURL.includes("127.0.0.1")) {
        // true - contains localhost or 127.0.0.1 add new serverURL
        newServerURL = 'http://127.0.0.1:3001/getRuleValue?ruleName=' + selectedRuleItemText;
    }
    // Just for testing on Mockoon - END //
    console.log("*** newServerURL: " + newServerURL);
    // get rule name from json global data data
    getvals(newServerURL).then(response => {
        if (response !== undefined) {
            console.log("json rule data loaded");
            //console.log(response);
            setRuleData(response);
        } else {
            Alert.render("Error!", "Error occurred during getting Rule Data!", errorColor);
        }
    });
}
/* Function to add table of valve */
function addValvesTable() {
    const tableDiv = document.getElementById("valves-div");
    tbl = document.createElement("table");
    tbl.setAttribute("class", "content-table");
    tbl.setAttribute("style", "width: 90%");
    row = document.createElement("tr");
    cell = document.createElement("td");
    cell.setAttribute("class", "td-padding");
    cell.setAttribute("colspan", "8");
    cell.appendChild(document.createTextNode("Select the Watering Valve"));
    row.appendChild(cell);
    tbl.appendChild(row);
    /* 
        <td class="td-padding">1.<br>
            <label class="switch">
                <input type="checkbox" class="valve-number" disabled>
                    <span class="slider round"></span>
            </label>
        </td>
     */
    row = document.createElement("tr");

    for (let i = 0; i < VALVES_NUMBER; i++) {
        cell = document.createElement("td");
        cell.setAttribute("class", "td-padding");
        cell.appendChild(document.createTextNode((i + 1) + "."));
        let br = document.createElement("br");
        cell.appendChild(br);
        /* cell.appendChild(document.createTextNode((i + 10) + "...")); */
        let label = document.createElement("label");
        label.setAttribute("class", "switch");
        input = document.createElement("input");
        input.type = "checkbox";
        input.setAttribute("id", "valve-number-" + i);
        input.disabled = true;
        let span = document.createElement("span");
        span.setAttribute("class", "slider round");
        label.appendChild(input);
        label.appendChild(span);
        cell.appendChild(label);
        row.appendChild(cell);
    }
    tbl.appendChild(row);
    tableDiv.appendChild(tbl);
}
/* Function to create inputs of valves thresholds */
function createThresholdsInput() {
    var row = document.getElementById("wetness-sensor-threshold");
    var i = 0;
    for (i = 0; i < VALVES_NUMBER; i++) {
        /* Adds cells with number of valve and input of value of threshold of valve */
        cell = document.createElement("td");
        cell.setAttribute("class", "td-padding");
        cell.appendChild(document.createTextNode((i + 1) + "."));
        let br = document.createElement("br");
        cell.appendChild(br);
        input = document.createElement("input");
        input.type = "text";
        input.size = 2;
        input.maxLength = 3;
        input.setAttribute("style", "text-align: center; width: 25px");
        input.setAttribute("id", "input-threshold-" + i);
        cell.appendChild(input);
        row.appendChild(cell);
    }

    row = document.getElementById("rain-sensor-threshold");
    cell = document.createElement("td");
    cell.setAttribute("class", "td-padding");
    input = document.createElement("input");
    input.type = "text";
    input.size = 1;
    input.maxLength = 3;
    input.setAttribute("style", "text-align: center;");
    input.setAttribute("id", "input-threshold-" + i);
    cell.appendChild(input);
    row.appendChild(cell);
}
/* Function to fill global setting inputs */
function setGlobalData(globalData) {
    const thresholdValues = globalData.threshold.split(";");
    // VALVES_NUMBER + 1, the +1 is for rain sensor threshold value
    for (var i = 0; i < VALVES_NUMBER + 1; i++) {
        /* Adds thresholds vaule to inputs */
        document.getElementById("input-threshold-" + i).value = thresholdValues[i];
        if (thresholdValues[i] > 0 && i < VALVES_NUMBER) {
            document.getElementById("valve-number-" + i).disabled = false;
        }
    }

    document.getElementById("wetness-sensitivity").value = globalData.wetness;
    document.getElementById("dryness-sensitivity").value = globalData.dryness;
    document.getElementById("refresh-interval").value = globalData.interval;
}
/* Function to fill rule setting inputs */
function setRuleData(data) {
    var ruleArray = data.rulevalue.split(";");
    // console.log(ruleArray);
    // Sets start date
    unixtimeToDate(document.getElementById("start-date"), ruleArray[0]);
    // Sets end date
    unixtimeToDate(document.getElementById("end-date"), ruleArray[1]);
    // Sets start time
    let hour = Math.floor(ruleArray[2] / 3600);
    let minute = Math.floor((ruleArray[2] % 3600) / 60);

    if (hour < 10) {
        hour = "0" + hour;
    }

    if (minute < 10) {
        minute = "0" + minute;
    }
    document.getElementById("start-time").value = hour + ":" + minute;
    // Sets duration time
    document.getElementById("duration-time").value = ((ruleArray[3] - ruleArray[2]) / 60);
    // Sets used valves 
    let valveBinary = dec2bin(ruleArray[4]);
    // console.log("valveBinary: " + valveBinary);
    for (let i = valveBinary.length - 1, j = 0; i >= 0; i--, j++) {
        // console.log("i: " + i + " - valve bin: " + valveBinary[i] + " - j: " + j);
        if (valveBinary[i] == "1") {
            document.getElementById("valve-number-" + j).checked = true;
            // elementCheckboxValveNumber[j].checked = true;
        } else {
            // elementCheckboxValveNumber[j].checked = false;
            document.getElementById("valve-number-" + j).checked = false;
        }
    }

    if (ruleArray[5] == "1")
        document.getElementById("soil-sensors-enabled").checked = true;
    else
        document.getElementById("soil-sensors-enabled").checked = false;

    if (ruleArray[6] == "1")
        document.getElementById("rain-sensor-enabled").checked = true;
    else
        document.getElementById("rain-sensor-enabled").checked = false;

    if (ruleArray[7] == "1") {
        document.getElementById("temp-sensor-enabled").checked = true;
        document.getElementById("min-temp").value = ruleArray[8];
        document.getElementById("max-temp").value = ruleArray[9];
        document.getElementById("min-temp").disabled = false;
        document.getElementById("max-temp").disabled = false;
    } else {
        document.getElementById("temp-sensor-enabled").checked = false;
        document.getElementById("min-temp").value = ruleArray[8];
        document.getElementById("max-temp").value = ruleArray[9];
        document.getElementById("min-temp").disabled = true;
        document.getElementById("max-temp").disabled = true;
    }
}
/* https://stackoverflow.com/questions/9873197/how-to-convert-date-to-timestamp */
// Function to set input date to unix time
function dateToUnixTime(element) {
    let date = document.getElementById(element).value;

    if (date == null || date == "") {
        Alert.render("Error!", "Invalid date! " + element.id, errorColor);
        document.getElementById(element).focus();
        return 0;
    }

    const dateArray = date.split("-");
    // var ut = new Date(dateArray[0], dateArray[1] - 1, dateArray[2], '23', '59', '59');
    var ut = new Date(dateArray[0], dateArray[1] - 1, dateArray[2]);
    return ut.getTime() / 1000;
}
/* Lambda function - checks if string alphanumeric , return true if is alphanumeric else false */
/* https://www.30secondsofcode.org/js/s/is-alpha-numeric */
const isAlphaNumeric = str => /^[a-z0-9]+$/gi.test(str);
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
/* Function to check if input value is numeric */
function isNumeric(element, minValue, maxValue) {
    // console.log("element.value: " + element.value);
    // console.log("parseInt(element.value, 10): " + parseInt(element.value, 10))
    if (element.value === "" || element.value === " " || element.value === "  " || element.value === "   " || isNaN(parseInt(element.value, 10)) ||
        (parseInt(element.value, 10) < parseInt(minValue, 10)) || (parseInt(element.value, 10) > parseInt(maxValue, 10))) {
        Alert.render("Error!", "Invalid value of " + element.id + ". <br>Min value: " + minValue + " - Max value: " + maxValue, errorColor);
        return false;
    }

    return true;
}
/* https://www.delftstack.com/howto/javascript/javascript-convert-timestamp-to-date/ */
// Function to convert unit time to date format
function unixtimeToDate(element, unix_timestamp) {
    console.log("unix_timestamp: " + unix_timestamp);
    var date = new Date(unix_timestamp * 1000);
    //console.log(date.toISOString());
    // date = date.toISOString().split("T");
    // element.value = date[0];
    let month = (date.getMonth() + 1) < 10 ? '0' + (date.getMonth() + 1) : (date.getMonth() + 1);
    let dayDate = date.getDay() < 10 ? '0' + date.getDate() : date.getDate();
    console.log(date.getFullYear() + "-" + month + "-" + dayDate);
    element.value = date.getFullYear() + "-" + month + "-" + dayDate;
    console.log(element.value);
}
/* Function for temperature checkbox */
function tempCheckBox(checkbox) {
    if (checkbox.checked) {
        document.getElementById("min-temp").disabled = false;
        document.getElementById("max-temp").disabled = false;
    } else {
        document.getElementById("min-temp").disabled = true;
        document.getElementById("max-temp").disabled = true;
    }
}
/* Function to check Global Settings of Watering System */
function checkGlobalSettings() {
    // Get threshold values an check these is numeric
    let thresholds = "";
    for (let i = 0; i < VALVES_NUMBER + 1; i++) {
        if (!isNumeric(document.getElementById("input-threshold-" + i), 0, 100)) {
            return false;
        }
        if (i < VALVES_NUMBER) {
            thresholds += document.getElementById("input-threshold-" + i).value + ";";
        } else {
            thresholds += document.getElementById("input-threshold-" + i).value;
        }
    }

    if (!isNumeric(document.getElementById("refresh-interval"), 5, 60)) {
        return false;
    }

    if (!isNumeric(document.getElementById("wetness-sensitivity"), 1, 2000)) {
        return false;
    }

    if (!isNumeric(document.getElementById("dryness-sensitivity"), 2001, 4095)) {
        return false;
    }
    document.getElementById("new-thresholds").value = thresholds;
    console.log(thresholds);
    // console.log("thresholds: " + document.getElementById("new-thresholds").value);
    if (document.getElementById("adm-pwd-2").value.length < 8) {
        Alert.render("Warning!", "Invalid administrator password.<br>Minimum 8 characters.", warningColor);
        return false;
    }

    return true;
}
/* Function to check the watering rule data */
function checkRuleData() {
    // Create rule string for saving
    let rule = "";
    // Gets values of start and end dates and convert are to unix timestamp
    let startDate = dateToUnixTime("start-date");
    if (startDate == 0)
        return false;

    let endDate = dateToUnixTime("end-date");
    if (endDate == 0)
        return false;

    if (startDate >= endDate) {
        Alert.render("Warning!", "Invalid date.<br>Start date is greater than End date.", warningColor);
        return false;
    }
    // Gets value of start time and convert it in unix timestamp format
    let startTime = document.getElementById("start-time").value;
    if (startTime == "" || startTime == null) {
        Alert.render("Warning!", "Invalid start time value.", warningColor);
        return false;
    }
    startTime = startTime.split(":");
    startTime = (startTime[0] * 3600) + (startTime[1] * 60);
    rule = startDate + ";" + endDate + ";" + startTime + ";";
    // Checks duration time value - is numeric - and compares it with refresh time interval of system
    if (!isNumeric(document.getElementById("duration-time"), document.getElementById("refresh-interval").value, 360)) {
        return false;
    }
    rule += (startTime + (document.getElementById("duration-time").value * 60)) + ";";
    // Checks which valves will be used and save these
    let valves = 0;
    for (let i = 0; i < VALVES_NUMBER; i++) {
        if (document.getElementById("valve-number-" + i).checked)
            valves += Math.pow(2, i);
    }
    rule += valves + ";";
    // Checks if wetness sensors enabled
    if (document.getElementById("soil-sensors-enabled").checked) {
        rule += "1;";
    } else {
        rule += "0;";
    }
    // Checks if rain sensor enabled
    if (document.getElementById("rain-sensor-enabled").checked) {
        rule += "1;";
    } else {
        rule += "0;";
    }
    // Checks if temperature sensor enabled
    if (document.getElementById("temp-sensor-enabled").checked) {
        if (!isNumeric(document.getElementById("min-temp"), -40, 4)) {
            return false;
        }

        if (!isNumeric(document.getElementById("max-temp"), 5, 50)) {
            return false;
        }

        if (parseInt(document.getElementById("min-temp").value, 10) >= parseInt(document.getElementById("max-temp").value, 10)) {
            Alert.render("Warning!", "The less value of temperature cannot be equal or greater than more value of temperature.", warningColor);
            return false;
        }

        rule += 1 + ";" + document.getElementById("min-temp").value + ";" + document.getElementById("max-temp").value;

    } else {
        rule += "0;0;0";
    }

    if (document.getElementById("adm-pwd").value.length < 8) {
        Alert.render("Warning!", "Invalid administrator password.<br>Minimum 8 characters.", warningColor);
        return false;
    }

    document.getElementById("new-rule-values").value = rule;
    console.log("rule: " + document.getElementById("new-rule-values").value);

    // document.getElementById("save-delete").value = "1";

    return true;
}
