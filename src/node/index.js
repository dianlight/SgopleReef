import "mini.css/src/flavors/mini-default.scss"
//import "mini.css/src/flavors/mini-dark.scss"
//import "mini.css/src/flavors/mini-nord.scss"
import "./flaticon.lineal.font"
import "./debug.css"
import "./holds.css"
import "./climate.less"
import "./favicon.ico"
import ReconnectingWebSocket from 'reconnecting-websocket';

import $ from "cash-dom";
import populate from "populate.js"

const CPROG = ['eco', 'normal', 'confort'];
const DAY_NAMES = ['mon', 'tue', 'wed', 'thu', 'fri', 'sat', 'sun'];

let displayTimeout;

function reload() {
    fetch("load")
        .then(response => response.json())
        .then(jdata => {
            populate($("#configForm")[0], jdata);
            $("#saveButtons").show();
        })
        .catch(err => console.error(err));
}
window.reload = reload;

function reloadWifi() {
    fetch("loadW")
        .then(response => response.json())
        .then(jdata => {
            populate($("#wifiConfigForm")[0], jdata);
            $("#connectButtons").show();
        })
        .catch(err => console.error(err));
}

function scanWifi() {
    $("#wifiScanButton").hide();
    $("#wifiScanResult").hide();
    $("#wifiScanTable").html("");
    $("#wifiScanSpinner").show();
    fetch("scanW")
        //  .then(response => response.json())  
        //  .then(jdata => {
        //    console.log(jdata);
        //  })
        .catch(err => console.error(err));
}

window.scanWifi = scanWifi;

function validateWifi() {
    //  console.log("Validate! ",$("#ssid").val() !== "" ,$("#wkey").val() !== "");
    if ($("#ssid").val() !== "" && $("#wkey").val() !== "") {
        $("#wifiConnectButton").removeAttr("disabled");
    } else {
        $("#wifiConnectButton").attr("disabled", "disabled");
    }
}
window.validateWifi = validateWifi;

async function saveConfig() {
    console.log($("#configForm")[0]);
    const data = new URLSearchParams(new FormData($("#configForm")[0]));
    console.log(data);
    await fetch("save", {
        method: 'POST',
        body: data
    });
    reload();
}

function save() {
    const currentPanel = $("#saveButtons").attr("data");
    console.log("Save on panel: ", currentPanel);
    if (currentPanel == "home") {
        // No save action
    } else if (currentPanel == "config") {
        saveConfig();
    }
}
window.save = save;

function change(day, hq) {
    $("#C" + day + "_" + hq).removeClass("unsaved");
    let cls = $("#C" + day + "_" + hq).attr("class");
    let p = CPROG.indexOf(cls);
    console.log("Letto", cls, p, CPROG);
    p = (p + 1) % 3;
    console.log(p, CPROG[p]);
    $("#C" + day + "_" + hq).attr("class", CPROG[p] + " unsaved");
}

window.change = change;

function show(currentPanel) {
    $(".panel").hide();
    $('header a').removeClass('bordered');
    $("#saveButtons").hide();
    $("#saveButtons").attr("data", currentPanel);
    $("#connectButtons").hide();
    $("footer").show();
    $("#" + currentPanel + "Tab").toggleClass("bordered");
    $("#" + currentPanel).show();
    if (currentPanel == "home") {
        refreshDisplayFromMap();
    } else if (currentPanel == "config") {
        clearTimeout(displayTimeout);
        reload();
    } else if (currentPanel == "wificonfig") {
        clearTimeout(displayTimeout);
        reloadWifi();
    } else if (currentPanel == "debug") {
        clearTimeout(displayTimeout);
        $("footer").hide();
        $("#debug-view")[0].scrollTop = $("#debug-view")[0].scrollHeight;
    }
}
window.show = show;

/*
async function wps() {
    console.log("WPS!");
    await fetch("wps");
    location.reload(true);
}
window.wps = wps;
*/

async function connect() {
    console.log($("#wifiConfigForm")[0]);
    const data = new URLSearchParams(new FormData($("#wifiConfigForm")[0]));
    console.log(data);
    await fetch("saveW", {
        method: 'POST',
        body: data
    });
    location.reload(true);
}
window.connect = connect;

function refreshDisplayFromMap() {
    var oReq = new XMLHttpRequest();
    oReq.open("GET", "screenpbm", true);
    oReq.responseType = "arraybuffer";

    oReq.onload = function(oEvent) {
        var BreakException = {};
        var arrayBuffer = oReq.response; // Note: not oReq.responseText
        if (arrayBuffer) {
            var byteArray = new Uint8Array(arrayBuffer);
            if (byteArray[0] != 0x50 || byteArray[1] != 0x34) {
                console.error("Only BPM P4 (Binary) are supported!")
                throw BreakException;
            }
            let x_size = 0,
                y_size = 0;
            let ascii_text = "";
            for (var i = 3; i < byteArray.byteLength; i++) {
                if (x_size == 0 && byteArray[i] == 0x0A) {
                    x_size = parseInt(ascii_text);
                    ascii_text = "";
                } else if (y_size == 0 && byteArray[i] == 0x20) {
                    y_size = parseInt(ascii_text);
                } else if (x_size == 0 || y_size == 0) {
                    ascii_text = ascii_text + String.fromCharCode(byteArray[i]);
                } else {
                    byteArray = byteArray.subarray(i);
                    break;
                }
            }
            //console.log("BPM Size " + x_size + "x" + y_size);

            let ctx = $("#screen")[0].getContext('2d');
            ctx.fillStyle = 'rgb(0, 0, 0)';
            let pos = 0;
            for (let y = 0; y < y_size; y++) {
                for (let x = 0; x < x_size; x += 8) {
                    for (var i = 7; i >= 0; i--) {
                        var bit = (byteArray[pos] & (1 << i)) > 0 ? 1 : 0;
                        if (bit == 1) ctx.fillStyle = 'rgb(0, 0, 0)';
                        else ctx.fillStyle = 'rgb(255, 255, 255)';
                        ctx.fillRect(x + (7 - i), y, 1, 1);
                    }
                    pos++;
                }
            }
            displayTimeout = setTimeout(refreshDisplayFromMap, 500);
        }
    };
    oReq.send(null);
}

function cmd(button) {
    fetch("cmd?cmd=" + $(button).attr("id").split("-")[0] + "&value=" + $(button).val())
        .then(result => reloadOstat(false))
        .catch(err => console.error(err));
}

window.cmd = cmd;

function reloadOstat(changePage = false) {
    // StartPage
    fetch("ostat")
        .then(response => response.json())
        .then(data => {
            if (changePage) {
                console.log(data, "First tab", data.otab);
                show(data.otab);
                if (data.bdg) {
                    alert(data.bdg);
                }
            }
            $("button[id|='mode'],button[id|='hold'],button[id|='away']").removeClass("primary").removeClass("secondary").removeClass("tertiary");
            $("button[id|='mode'][value='" + data.mode + "']").addClass($("button[id|='mode'][value='" + data.mode + "']").data('onclass'));
            //            $("button[id|='hold'][value='" + data.hold + "']").addClass($("button[id|='hold'][value='" + data.hold + "']").data('onclass'));
            //            $("#away-" + (data.away ? "on" : 'off')).addClass("primary");
        })
        .catch(err => console.error(err));
}

function debug(message) {
    $("#debug-view").val($("#debug-view").val() + message);
    if ($("#autoscroll").is(":checked")) {
        $("#debug-view")[0].scrollTop = $("#debug-view")[0].scrollHeight;
    }
}


$().ready(function() {

    reloadOstat(true);

    // WebSockets
    var debug_service = new ReconnectingWebSocket('ws://' + window.location.host + '/log');
    debug_service.onmessage = function(event) {
        //        console.log("Messaggio", event);
        debug(event.data);
    }
    debug_service.onopen = function(event) {
        //        console.log(event);
        debug('Connected\n');
        debug_service.send("CSL");
    }
    debug_service.onclose = function() {
        debug('Disconnected\n');
    }
    debug_service.onerror = function() {
        debug('Error!\n');
    }
    $("#debug-view").on("scroll", (elem) => {
        //    console.log(elem);
        //    console.log($("#debug-view")[0].scrollTop + $("#debug-view").height(), $("#debug-view")[0].scrollHeight)
        if ($("#debug-view")[0].scrollTop + $("#debug-view").height() <= $("#debug-view")[0].scrollHeight - 4) {
            $("#autoscroll")[0].checked = false;
        }
    });


    var scan_service = new ReconnectingWebSocket('ws://' + window.location.host + '/scan');
    scan_service.onmessage = function(event) {
        let item = JSON.parse(event.data);
        console.log("Got Scan Result", event.data);
        $("#wifiScanSpinner").hide();
        if (item.end) {
            $("#wifiScanButton").show();
            validateWifi();
        } else {
            // Search networks
            //      console.log($("#wifiScanTable tr td[data-ssid='"+item.ssid+"'][data-label='SSID']"));
            if ($("#wifiScanTable tr td[data-ssid='" + item.ssid + "'][data-label='SSID']").length != 0) {
                console.log("Duplicate Network", item);
                let str = $("#wifiScanTable tr td[data-ssid='" + item.ssid + "'][data-label='Strength']");
                str.text(str.text() + "/" + item.dBm + "dBm");
            } else {
                let html = "<tr id=\"netw" + item.i + "\">";
                html += "<td data-label=\"SSID\" data-ssid=\"" + item.ssid + "\">" + item.ssid + "</td>";
                html += "<td data-Label=\"Strength\" data-ssid=\"" + item.ssid + "\">" + item.dBm + "dBm</td>";
                html += "<td data-label=\"Type\" data-ssid=\"" + item.ssid + "\">" + (item.open ? "" : "<span class=\"icon-lock\"></span>") + "</td>";
                html += "</tr>";
                $("#wifiScanTable").append(html);
                $("#netw" + item.i).on('click', event => {
                    console.log(item.ssid);
                    $("#ssid").val(item.ssid)[0].scrollIntoView();
                    $("#wkey")[0].focus();
                });
                $("#wifiScanResult").show();
            }
        }
    }
    scan_service.onopen = function(event) {
        console.log("Connect WS /scan", event);
        scan_service.send("CSL2");
    }
    scan_service.onclose = function() {
        console.log("Disconect WS /scan");
    }
    scan_service.onerror = function() {
        console.log("Error WS /scan");
    }

});