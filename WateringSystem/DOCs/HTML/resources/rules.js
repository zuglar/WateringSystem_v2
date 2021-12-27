var rulesArrayLength = 0;

/* Load navbar from htm file */
$(document).ready(function () {
    $('.navbar').load("./resources/navbar.htm");

    var path = window.location.pathname;
    page = path.split("/").pop();
    console.log("page: " + page);

    if (page == "rules.htm") {
        setRulesNameInDropDownButton();
    }
});

function setRulesNameInDropDownButton() {
    var element = document.getElementById("rules-name");
    if(element.textContent.localeCompare("%RULES_VALUE%")) {
        element.disabled = true;
        alert("\t\tError!!!\nYou don't have crearted watering rule(s).\nYou have to create it first.");
        return;
    }

    element.disabled = false;
    const rulesArray = element.textContent.split(";");
    rulesArrayLength = rulesArray.length;

    const rulesSelect = document.getElementById("rules-select");
    for(var i = 0; i < rulesArrayLength; i++) {
        var opt = document.createElement("option");
        opt.innerHTML = rulesArray[i];
        opt.value = i;
        rulesSelect.add(opt);
    }
}

function selectedRule(element) {
    var selectedItem = document.getElementById(element.id).value;
    alert("Selected RULE id: " + selectedItem);
    showRuleValue(selectedItem);
}

function showRuleValue(item) {

}

function saveRules() {

}