/* Import main functions, variables from main.js */
import { scalePage, getvals } from '/resources/main.js';

var serverIP;   // IP address of server
var serverURL;  // URL of server

$(document).ready(function () {
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
 
        showMessage();
    });
})

function showMessage() {
    var count = 40;

    var newServerURL;
    // Just for testing on Mockoon - START //
    if (serverURL.includes("localhost") || serverURL.includes("127.0.0.1")) {
        // true - contains localhost or 127.0.0.1 add new serverURL;
        newServerURL = "http://127.0.0.1:3001/restart";
        // Just for testing on Mockoon - END //
    } else {
        newServerURL = serverURL + "/restart";
    }

    fetch(newServerURL, {
        method: "POST",
        body: 'restart=1',
        headers: {
            'Content-type': 'application/x-www-form-urlencoded'
        },
        mode: "cors",
        cache: "default"
    }).then((response) => {
        // console.log(response);
        if (!response.ok) {
            throw Error(response.statusText + " - " + response.url);
        }
        return response.statusText;
    }).then((responseData) => {
        // console.log(responseData);
        return responseData;
    }).catch(error => {
        console.warn(error);
    });

    var time = setInterval(function () {
        count--;
        document.getElementById("message").innerHTML = "<br>The system has been restarted.<br><br>Please wait ";
        document.getElementById("count").innerHTML = count;
        if (count > 1) {
            document.getElementById("sec").innerHTML = "seconds.";
        } else {
            document.getElementById("sec").innerHTML = "second.";
        }

        if (count == 0) {
            clearInterval(time);
            window.location.replace(window.document.referrer);
        }
    }, 1000);
}
