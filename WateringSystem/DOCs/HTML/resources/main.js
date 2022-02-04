/* Global variables */
var warningColor = "#ff9800";
var errorColor = "#f44336";
var okColor = "#04AA6D";

/* Function to calculate scale value for page center on screen depends on screen size*/
function scalePage() {
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
}
/* Function Fetching the data */
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
/* https://www.developphp.com/video/JavaScript/Custom-Confirm-Box-Programming-Tutorial */
/* Class Custom Alert Dialog*/
class CustomAlertDialog {
    constructor() { }

    render(title, message, color) {
        var winW = window.innerWidth;
        var winH = window.innerHeight;

        document.getElementById('dialogboxhead').innerHTML = title;
        document.getElementById('dialogboxbody').innerHTML = message;
        document.getElementById('dialogboxfoot').innerHTML = '<button id="alert" style="background:' + color + '">OK</button>';
        document.getElementById('dialogboxhead').style.background = color;
        document.getElementById('dialogboxbody').style.background = color;
        document.getElementById('dialogboxbody').style.opacity = 0.7;
        document.getElementById('dialogboxfoot').style.background = color;
        document.getElementById('alert').onclick = this.ok;

        var dialogoverlay = document.getElementById('dialogoverlay');
        var dialogbox = document.getElementById('dialogbox');
        dialogoverlay.style.display = "block";
        dialogoverlay.style.height = winH + "px";
        dialogbox.style.display = "block";

        dialogbox.style.left = (winW / 2) - (dialogbox.offsetWidth * .5) + "px";
        dialogbox.style.top = (winH / 2) - (dialogbox.offsetHeight * .5) + "px";
    }

    ok() {
        document.getElementById('dialogbox').style.display = "none";
        document.getElementById('dialogoverlay').style.display = "none";
    }
}
/* Class Custom Confirm */
class CustomConfirmDialog {
    constructor() { }

    render = function (type, title, message, color, callback) {
        var winW = window.innerWidth;
        var winH = window.innerHeight;

        document.getElementById('dialogboxhead').innerHTML = title;
        document.getElementById('dialogboxbody').innerHTML = message;
        if (type === 1) {
            document.getElementById('dialogboxfoot').innerHTML = '<button id="yes" style="background:' + color + '">Yes</button>' +
                '<button id="no" style="background:' + color + '">No</button>';
        } else if (type === 2) {
            document.getElementById('dialogboxfoot').innerHTML = '<button id="yes" style="background:' + color + '">Save</button>' +
                '<button id="no" style="background:' + color + '">Delete</button>' +  '<button id="cancel" style="background:' + color + '">Cancel</button>';
            document.getElementById('cancel').addEventListener('click', () => {
                this.closeDialog();
                callback(2);
            })
        }

        document.getElementById('yes').addEventListener('click', () => {
            this.closeDialog();
            callback(1);
        });

        document.getElementById('no').addEventListener('click', () => {
            this.closeDialog();
            callback(0);
        });

        document.getElementById('dialogboxhead').style.background = color;
        document.getElementById('dialogboxbody').style.background = color;
        document.getElementById('dialogboxbody').style.opacity = 0.7;
        document.getElementById('dialogboxfoot').style.background = color;

        var dialogoverlay = document.getElementById('dialogoverlay');
        var dialogbox = document.getElementById('dialogbox');
        dialogoverlay.style.display = "block";
        dialogoverlay.style.height = winH + "px";
        dialogbox.style.display = "block";

        dialogbox.style.left = (winW / 2) - (dialogbox.offsetWidth * .5) + "px";
        dialogbox.style.top = (winH / 2) - (dialogbox.offsetHeight * .5) + "px";
    }

    closeDialog() {
        document.getElementById('dialogbox').style.display = "none";
        document.getElementById('dialogoverlay').style.display = "none";
    }
}

export { warningColor, errorColor, okColor, scalePage, getvals, CustomAlertDialog, CustomConfirmDialog };
