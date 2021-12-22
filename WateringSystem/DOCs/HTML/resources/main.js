$(function () {
    /* Variables for main widow size and element size */
    var windowWidth = $(window).width();
    var mainDivWidth = document.getElementById("main").offsetWidth;

    if (windowWidth > mainDivWidth) {
        $("#main").css({
            left: ((windowWidth / 2) - (mainDivWidth / 2)) + "px"
        });
    }
    else {
        var scale = windowWidth / mainDivWidth;
        document.querySelector('meta[name="viewport"]').setAttribute('content', 'width=' + mainDivWidth + ', initial-scale=' + scale + '');
    }

    var path = window.location.pathname;
    var page = path.split("/").pop();
    console.log(page);

    alert("--- " +window.location.pathname);
});

$(document).ready(function () {
    $('.navbar').load("./navbar.htm");
});

/* window.onload = function () {
    if (document.querySelectorAll("section:target").length == 0) {
        window.location = "#home";
    }
} */