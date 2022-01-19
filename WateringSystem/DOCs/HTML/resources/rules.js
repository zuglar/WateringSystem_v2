/* Global Variables */
var rulesNameArray; // array contains the names of rule stored in %RULES_NAME%
var rulesNameArrayLength;
var selectedRuleItemText;   // name of rule selected by select element
var rulesValueArray;    // array contains the values of rule for each rule stored in  %RULES_VALUE%
var rulesValueArrayLength;
// array contains the values of threshold of wetness and rain sensors stored in %THRESHOLD_VALUES%
var thresholdValuesArray;
var thresholdValuesArrayLenght;
// array of elements which contains the created "input text" elements by class name "input-threshold-value" 
var inputElementThresholdsArray;
// array of elements which contains the created "input checkbox" elements by class name "valve-number"
var elementCheckboxValveNumber;
var tbl;    // variable for table element
var row;    // variable for tr element
var cell;   // variable for td element
var input;  // variable for input element

var serverIP;
var serverURL;
var ruleArray;

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
            if (serverURL.indexOf("http://") !== 0) {
                // true - not contain http:// we add it
                serverURL = "http://" + serverURL;
            }
            console.log("4. serverURL: " + serverURL);

            if (page == "rules.htm") {
                addRulesName();
            }
        })
});


/* 
$(window).on('load', function () {
    // alert("Page is loaded.");
    var path = window.location.pathname;
    page = path.split("/").pop();
    console.log("page: " + page);

    if (page == "rules.htm") {
        // addRulesName();
        // addValvesTable();
        // addThresholdValueInput();
        // selectedRule();

    }
}); */
/* Function to add name of rule to select element, values of threshold, values of 
maximum wetness and dryness and value of time of system refresh interval */
function addRulesName() {
    const rulesSelect = document.getElementById("rules-select");
    let getGlobalDataURL = serverURL + '/getGlobalData';
    $.getJSON(getGlobalDataURL, function (data) {
        console.log("json rule data loaded");
        if (!data.hasOwnProperty("rulename")) {
            alert("\tERROR!!!\nUnable to read Rule settings data!!!");
            return;
        }
    }).done(function (data) {
        addValvesTable();
        addGlobalSettings(data);
        const ruleNames = data.rulename;
        for (var i = 0; i < ruleNames.length; i++) {
            var opt = document.createElement("option");
            opt.innerHTML = ruleNames[i];
            opt.value = i;
            opt.id = "select-option-" + i;
            rulesSelect.add(opt);
        }
        console.log("option: " + document.getElementById("select-option-0").textContent);
        selectedRule();
    }).fail(function () {
        alert("Error occurred !!! ");
        window.location.href = "./resources/notfound.htm";
    })
}

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

    // console.log("*** serverURL: " + serverURL);
    // var ruleText = document.getElementById("rules-select").options[selectedItemIndex].text;
    // console.log("text: " + ruleText);
    // code for testing on localhost with mockoon
    // let getRuleValueURL = serverURL + "/getRuleValue"; //?ruleName=" + selectedRuleItemText;
    // code for server
    let getRuleValueURL = serverURL + "/getRuleValue?ruleName=" + selectedRuleItemText;
    console.log("*** getRuleValueURL: " + getRuleValueURL);
    $.getJSON(getRuleValueURL, function (data) {
        console.log("json rule value loaded");
        if (!data.hasOwnProperty("rulevalue")) {
            alert("\tERROR!!!\nUnable to read Rule Value data!!!");
            window.location.href = "./resources/notfound.htm";
            return;
        }
    }).done(function (data) {
        // console.log("data.rulevalue: " + data.rulevalue);
        ruleArray = data.rulevalue.split(";");
        // console.log("ruleArray.length: " + ruleArray.length);
        // console.log("elementCheckboxValveNumber: " + elementCheckboxValveNumber + " - length: " + elementCheckboxValveNumber.length);

        var trhresholdsElement = document.getElementsByClassName("input-threshold-value");
        // console.log("****trhresholdsElement: " + trhresholdsElement + " ****length: " + trhresholdsElement.length + "--- 0: " + trhresholdsElement[0].value);

        for (let i = 0; i < ruleArray.length; i++) {
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
                    elementCheckboxValveNumber[j].checked = true;
                } else {
                    elementCheckboxValveNumber[j].checked = false;
                }

                if (trhresholdsElement[j].value != 0) {
                    elementCheckboxValveNumber[j].disabled = false;
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
    }).fail(function () {
        alert("Error occurred !!! ");
        window.location.href = "./resources/notfound.htm";
    })
}
/* Function to save the watering rule */
function saveRule() {
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
        alert("\t\tERROR!!!\nInvalid date.Start date is greater than End date.");
        return false;
    }
    // Gets value of start time and convert it in unix timestamp format
    let startTime = document.getElementById("start-time").value;
    if (startTime == "" || startTime == null) {
        alert("\t\tERROR!!!\nInvalid start time value.");
        document.getElementById("start-time").focus();
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
    for (let i = 0; i < document.getElementsByClassName("valve-number").length; i++) {
        if (document.getElementsByClassName("valve-number")[i].checked)
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
            alert("The less value of temperature cannot be equal or greater than more value of temperature.");
            document.getElementById("min-temp").focus();
            return false;
        }

        rule += 1 + ";" + document.getElementById("min-temp").value + ";" + document.getElementById("max-temp").value;

    } else {
        rule += "0;0;0";
    }

    console.log("rule: " + rule);

    if (ruleArray.length > 10) {
        alert("You cannot save more than 10 watering rules !");
        return false;
    }

    if (document.getElementById("adm-pwd").value.length < 8) {
        alert("\tERROR !\nMinimum 8 charachters.");
        document.getElementById("adm-pwd").focus();
        return false;
    }

    document.getElementById("new-rule-values").value = rule;
    // console.log("rule: " + document.getElementById("new-rule-values").value);
    // console.log(document.getElementById("rule-name-input").value);

    document.getElementById("save-delete").value = "1";

    /* Alert to confirm to save new rule values */
    if (!window.confirm("Are you sure you want to save new settings?")) {
        alert("The new settings has not been saved!");
        return false;
    }

    return true;
}
/* Function to delete rule */
function deleteRule() {
    if (document.getElementById("adm-pwd").value.length < 8) {
        alert("\tERROR !\nMinimum 8 charachters.");
        document.getElementById("adm-pwd").focus();
        return false;
    }

    if (!window.confirm("\t\tWARNING!!!\nDo you want to delete rule: \"" + selectedRuleItemText + "\"?")) {
        return false;
    }
    document.getElementById("save-delete").value = "0";
    return true;
}
/* Function to save Global Settings of Watering System */
function saveGlobalSettings() {
    // Get threshold values an check these is numeric
    let thresholds = "";
    for (let i = 0; i < inputElementThresholdsArray.length; i++) {
        if (!isNumeric(inputElementThresholdsArray[i], 0, 100)) {
            return false;
        }
        if (i < (inputElementThresholdsArray.length - 1)) {
            thresholds += inputElementThresholdsArray[i].value + ";";
        } else {
            thresholds += inputElementThresholdsArray[i].value;
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
    // console.log("thresholds: " + document.getElementById("new-thresholds").value);
    if (document.getElementById("adm-pwd-2").value.length < 8) {
        alert("\tERROR !\nMinimum 8 charachters.");
        document.getElementById("adm-pwd-2").focus();
        return false;
    }

    /* Alert to confirm to save new global settings */
    if (!window.confirm("Are you sure you want to save new settings?")) {
        alert("The new settings has not been saved!");
        return false;
    }

    return true;
}
/* Function to add table of valve */
function addValvesTable() {
    const tableDiv = document.getElementById("valves-div");
    tbl = document.createElement("table");
    tbl.classList.add("content-table");
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

    for (let i = 0; i < 8; i++) {
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
        input.setAttribute("class", "valve-number");
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
    // Gets element by class name = "valve-number"
    elementCheckboxValveNumber = document.getElementsByClassName("valve-number");
}
/* Function to create inputs of threshold values of sensors of wettness */
function addGlobalSettings(globalData) {
    const thresholdValues = globalData.threshold.split(";");
    // console.log("thresholdValues: " + thresholdValues);
    var row = document.getElementById("wetness-sensor-threshold");
    var i = 0;
    for (i = 0; i < thresholdValues.length - 1; i++) {
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
        input.value = thresholdValues[i];
        input.setAttribute("style", "text-align: center;");
        input.setAttribute("class", "input-threshold-value")
        cell.appendChild(input);
        row.appendChild(cell);
        /* var x = document.getElementsByClassName("input-threshold-value");
        alert(x[i].value); */
    }

    row = document.getElementById("rain-sensor-threshold");
    cell = document.createElement("td");
    cell.setAttribute("class", "td-padding");
    input = document.createElement("input");
    input.type = "text";
    input.size = 2;
    input.maxLength = 3;
    input.value = thresholdValues[i];
    input.setAttribute("style", "text-align: center;");
    input.setAttribute("class", "input-threshold-value")
    cell.appendChild(input);
    row.appendChild(cell);

    // Gets element by class name = "input-threshold-value"
    inputElementThresholdsArray = document.getElementsByClassName("input-threshold-value");

    // console.log("globalData.wetness: " + globalData.wetness);
    // console.log("globalData.dryness: " + globalData.dryness);
    // console.log("globalData.interval: " + globalData.interval);

    document.getElementById("wetness-sensitivity").value = globalData.wetness;
    document.getElementById("dryness-sensitivity").value = globalData.dryness;
    document.getElementById("refresh-interval").value = globalData.interval;

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
    
    if (element.value === "" || element.value === " " || element.value === "  " || element.value === "   " ||
        (parseInt(element.value, 10) < parseInt(minValue, 10)) || (parseInt(element.value, 10) > parseInt(maxValue, 10))) {
        alert("\t\tERROR!!!\nInvalid value.You have to check it.\nMin value: " + minValue + " - Max value: " + maxValue);
        element.focus();
        return false;
    }

    return true;
}
// Function to set input date to unix time
function dateToUnixTime(element) {
    let date = document.getElementById(element).value;

    if (date == null || date == "") {
        alert("\t\tERROR!!!\nInvalid date.You have to check it.");
        document.getElementById(element).focus();
        return 0;
    }

    const dateArray = date.split("-");
    var ut = new Date(dateArray[0], dateArray[1] - 1, dateArray[2], '23', '59', '59');
    return ut.getTime() / 1000;
}
// Function to convert unit time to date format
function unixtimeToDate(element, unix_timestamp) {
    var date = new Date(unix_timestamp * 1000);
    //console.log(date.toISOString());
    date = date.toISOString().split("T");
    element.value = date[0];
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
