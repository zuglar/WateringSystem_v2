/* Variables */
var page = "index.htm"

$(function () {
    /* Variables for main widow size and element size */
    var windowWidth = $(window).width();
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

    var path = window.location.pathname;
    page = path.split("/").pop();
    console.log(page);

    if (page == "" || page == "index.htm") {
        valvesState();
        wetnessRainSensorsState();
    }
});
/* Load navbar from htm file */
$(document).ready(function () {
    $('.navbar').load("./navbar.htm");
});
/* Function to show states of valves */
function valvesState() {
    var valves = document.getElementById("valves").textContent;
    console.log("valves: " + valves);
    if (valves.length != 8) {
        alert("Warning!!!\nUnable to get data of state of valves!");
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
        alert("ERROR!!!\nUnable to read wetness and rain sensors value!!!");
        return;
    }

    var wetnessSensorNumber = 8;

    var wetness = document.getElementById("wetness-state");
    var rain = document.getElementById("rain-state");

    for (let i = 0; i < valueArray.length; i++) {
        var ttd = document.createElement("td");
        /* ttd.setAttribute("class", "t8c_col"); */

        if (i < wetnessSensorNumber) {
            ttd.innerHTML = valueArray[i] + "%";
            wetness.appendChild(ttd);
        } else {
            var img = document.createElement('img');
            /* img.setAttribute("style", "margin-left: auto; margin-right: auto"); */
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
/* Refresh sensors data and valves statein every 2 seconds  */
getData();
setInterval(() => {
    if (page == "index.htm") {
        getData();
    }
}, 2000);

function getData() {
    var xhr = new XMLHttpRequest();
    /* xhr.open("GET", "/update?output="+element.id+"&state=1", true); */
}