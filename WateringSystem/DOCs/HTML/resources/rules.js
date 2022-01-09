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

/* Load navbar from htm file */
$(document).ready(function () {
    $('.navbar').load("./resources/navbar.htm");

    /* var path = window.location.pathname;
    page = path.split("/").pop();
    console.log("page: " + page); */

    /*     if (page == "rules.htm") {
            addRulesName();
            addValvesTable();
            addThresholdValueInput();
        } */
});

$(window).on('load', function () {
    // alert("Page is loaded.");
    var path = window.location.pathname;
    page = path.split("/").pop();
    console.log("page: " + page);

    if (page == "rules.htm") {
        addRulesName();
        addValvesTable();
        addThresholdValueInput();
        selectedRule();
    }
});
/* Function to add name of rule to select element */
function addRulesName() {
    const elementRuleName = document.getElementById("rules-name");
    rulesNameArray = elementRuleName.textContent.split(";");
    rulesNameArrayLength = rulesNameArray.length;

    if ((rulesNameArrayLength == 0) || (rulesNameArrayLength == 1 && !isAlphaNumeric(elementRuleName.textContent))) {
        elementRuleName.disabled = true;
        alert("\t\tError!!!\nYou don't have crearted watering rule(s).\nYou have to create it first.");
        return;
    }

    const elementRuleValue = document.getElementById("rules-value");
    rulesValueArray = elementRuleValue.textContent.split("-");
    rulesValueArrayLength = rulesValueArray.length;

    // console.log("rulesNameArrayLength: " + rulesNameArrayLength + " - rulesValueArrayLength: " + rulesValueArrayLength);

    if (rulesNameArrayLength != rulesValueArrayLength) {
        elementRuleName.disabled = true;
        alert("\t\tError!!!\nInvalid data loaded.\nRules name and Rules value are not the same. ");
        return;
    }

    elementRuleName.disabled = false;

    const rulesSelect = document.getElementById("rules-select");
    for (var i = 0; i < rulesNameArrayLength; i++) {
        var opt = document.createElement("option");
        opt.innerHTML = rulesNameArray[i];
        opt.value = i;
        rulesSelect.add(opt);
    }
    // Set first option as default - selected
    rulesSelect.value = 0;

    const thresholdString = document.getElementById("threshold-values").textContent;
    thresholdValuesArray = thresholdString.split(";");
    thresholdValuesArrayLenght = thresholdValuesArray.length;
    // console.log("thresholdValuesArrayLenght: " + thresholdValuesArrayLenght);
}

function selectedRule() {
    // Gets option value from select element
    var selectedItemIndex = document.getElementById("rules-select").value;
    // console.log("Selected RULE index: " + selectedItemIndex);
    // Gets option text from select element
    selectedRuleItemText = document.getElementById("rules-select").options[selectedItemIndex].text;
    // console.log("Selected RULE text: " + selectedRuleItemText);
    // Set rule name to input elemet 
    // https://stackoverflow.com/questions/1085801/get-selected-value-in-dropdown-list-using-javascript?rq=1
    document.getElementById("rule-name-input").value = selectedRuleItemText;

    // console.log("inputElementThresholdsArray: " + inputElementThresholdsArray + " - length: " + inputElementThresholdsArray.length);
    // console.log("elementCheckboxValveNumber: " + elementCheckboxValveNumber + " - length: " + elementCheckboxValveNumber.length);
    // Checks threshold value array length and fills the required inputs
    if (thresholdValuesArrayLenght == 9) {
        // Fills in threshold values and if the some value is 0 than this valve cannot be set
        for (let i = 0; i < thresholdValuesArrayLenght; i++) {
            inputElementThresholdsArray[i].value = thresholdValuesArray[i];
            if (i < 8) {
                if (thresholdValuesArray[i] != 0) {
                    elementCheckboxValveNumber[i].disabled = false;
                }
            }
        }

        const selectedRuleValue = rulesValueArray[selectedItemIndex];
        // console.log("selectedRuleValue: " + selectedRuleValue);
        const selectedRuleArray = selectedRuleValue.split(";");
        // console.log("selectedRuleArray.length: " + selectedRuleArray.length);
        // Sets start date
        unixtimeToDate(document.getElementById("start-date"), selectedRuleArray[0]);
        // Sets end date
        unixtimeToDate(document.getElementById("end-date"), selectedRuleArray[1]);
        // Sets start time
        let hour = Math.floor(selectedRuleArray[2] / 3600);
        let minute = Math.floor((selectedRuleArray[2] % 3600) / 60);

        if (hour < 10) {
            hour = "0" + hour;
        }

        if (minute < 10) {
            minute = "0" + minute;
        }
        document.getElementById("start-time").value = hour + ":" + minute;
        // Sets duration time
        document.getElementById("duration-time").value = Math.floor((selectedRuleArray[3] - selectedRuleArray[2]) / 60);
        // Sets used valves 
        let valveBinary = dec2bin(selectedRuleArray[4]);
        // console.log("valveBinary: " + valveBinary);
        for (let i = valveBinary.length - 1, j = 0; i >= 0; i--, j++) {
            // console.log("i: " + i + " - valve bin: " + valveBinary[i] + " - j: " + j);
            if (valveBinary[i] == "1") {
                elementCheckboxValveNumber[j].checked = true;
            } else {
                elementCheckboxValveNumber[j].checked = false;
            }
        }

        if (selectedRuleArray[5] == "1") {
            document.getElementById("soil-sensors-enabled").checked = true;
        } else {
            document.getElementById("soil-sensors-enabled").checked = false;
        }

        if (selectedRuleArray[6] == "1") {
            document.getElementById("rain-sensor-enabled").checked = true;
        } else {
            document.getElementById("rain-sensor-enabled").checked = false;
        }
    }
}
/* Function to save or modify the watering rule */
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
    if (!isNumeric(document.getElementById("duration-time").value, document.getElementById("duration-time"),
        document.getElementById("refresh-interval").value, 360)) {
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
        rule += "1";
    } else {
        rule += "0";
    }

    console.log("rule: " + rule);

    if (document.getElementById("adm-pwd").value.length < 8) {
        alert("\tERROR !\nMinimum 8 charachters.");
        document.getElementById("adm-pwd").focus();
        return false;
    }

    document.getElementById("new-rule-values").value = rule;
    // console.log("rule: " + document.getElementById("new-rule-values").value);
    // console.log(document.getElementById("rule-name-input").value);
    // condition to check if we have a saved rule with name like the new rule name
    if (document.getElementById("rule-name-input").value.localeCompare(selectedRuleItemText) == 0) {
        if (window.confirm("The rule name: \"" + selectedRuleItemText + "\" exists.\nDo you want to modify values of rule?")) {
            document.getElementById("save-modify-delete").value = "0";
            return true;
        } else {
            return false;
        }
    }
    document.getElementById("save-modify-delete").value = "1";

    return false;
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
    document.getElementById("save-modify-delete").value = "-1";
    return true;
}
/* Function to save Global Settings of Watering System */
function saveGlobalSettings() {
    // Get threshold values an check these is numeric
    for (let i = 0; i < inputElementThresholdsArray.length; i++) {
        if (!isNumeric(inputElementThresholdsArray[i].value, inputElementThresholdsArray[i], 0, 100)) {
            return false;
        }
    }

    if (!isNumeric(document.getElementById("refresh-interval").value, document.getElementById("refresh-interval"), 5, 60)) {
        return false;
    }

    if (!isNumeric(document.getElementById("wetness-sensitivity").value, document.getElementById("wetness-sensitivity"), 1, 4095)) {
        return false;
    }

    if (!isNumeric(document.getElementById("dryness-sensitivity").value, document.getElementById("dryness-sensitivity"), 1, 4095)) {
        return false;
    }

    if (document.getElementById("adm-pwd-2").value.length < 8) {
        alert("\tERROR !\nMinimum 8 charachters.");
        document.getElementById("adm-pwd-2").focus();
        return false;
    }

    return false;
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
    // Gets element by class name = "input-threshold-value"
    elementCheckboxValveNumber = document.getElementsByClassName("valve-number");
}
/* Function to create inputs of threshold values of sensors of wettness */
function addThresholdValueInput() {
    var row = document.getElementById("wetness-sensor-threshold");
    for (let i = 0; i < 8; i++) {
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
        input.value = 0;
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
    input.value = 0;
    input.setAttribute("style", "text-align: center;");
    input.setAttribute("class", "input-threshold-value")
    cell.appendChild(input);
    row.appendChild(cell);

    // Gets element by class name = "input-threshold-value"
    inputElementThresholdsArray = document.getElementsByClassName("input-threshold-value");

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
function isNumeric(num, element, minValue, maxValue) {
    var result = true;

    if (num == "" || num == " " || num == "  " || num == "   ") {
        result = false;
    } else {
        if (typeof num != "string") {
            result = false;
        } else {
            if (isNaN(num)) {
                result = false;
            } else {
                if (parseInt(num, 10) < parseInt(minValue, 10) || parseInt(num, 10) > parseInt(maxValue, 10)) {
                    result = false;
                }
            }
        }
    }

    if (!result) {
        alert("\t\tERROR!!!\nInvalid value.You have to check it.\nMin value: " + minValue + " - Max value: " + maxValue);
        element.focus();
    }
    return result;
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
    // console.log(date.toISOString());
    date = date.toISOString().split("T");
    element.value = date[0];
}
