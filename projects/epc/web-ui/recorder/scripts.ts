var bars = []
var messageHandler = function (event) { };
var updateWaveform = false;
var zoom = 1.0;
var scrollX = -1;
var lastBarIdToShow = -1; // -1 = autoscroll
var timingInfo = {
    serverTime: 0,
    localTime: 0
};

var playbackRange = {
    begin: 0, end: 0
}

enum Dragging {
    noDrag,
    dragPane,
    dragLeftBorder,
    dragRightBorder,
    dragRange
}

var dragging = Dragging.noDrag;


function formatMemSize(s) {
    var i = 0;
    var units = [' B', ' kB', ' MB'];

    while (s > 1024 && i < 2) {
        s = s / 1024;
        i++;
    }

    return Math.max(s, 0.1).toFixed(1) + units[i];
};

function onGotInfo(info, webSocket) {
    timingInfo.serverTime = info.time;
    timingInfo.localTime = Date.now() * 1000 * 1000;

    document.getElementById("first-available-frame-timestamp").textContent = buildTime(info.storage.first.recordTime);
    document.getElementById("last-available-frame-timestamp").textContent = buildTime(info.storage.last.recordTime);

    document.getElementById("consumed-mem").textContent = formatMemSize(info.storage.memUsage.current);
    document.getElementById("max-mem").textContent = formatMemSize(info.storage.memUsage.max);

    document.getElementById("recording-state").textContent = info.recorder.paused ? "rec paused" : "recording...";
    document.getElementById("playback-state").textContent = info.player.paused ? "playback paused" : "playing back...";

    // remove outdated bars
    while (bars.length > 0) {
        if (bars[0].id >= info.storage.first.id)
            break;

        bars[0].shift();
        updateWaveform = true;
    }

    // query new bars
    var nextId = info.storage.first.id;

    if (bars.length > 0)
        nextId = bars[bars.length - 1].id + 1;

    if (info.storage.last.id - nextId < 1) {
        setTimeout(function () { update(webSocket); }, 200);
        return;
    }

    messageHandler = (e) => onGotQuery(e, webSocket);
    webSocket.send(JSON.stringify({ "query-frames": { "begin": nextId, "end": info.storage.last.id } }));
}

function onGotQuery(info, webSocket) {
    info.forEach(function (a) {
        bars.push(a);
        updateWaveform = true;
    });

    if (updateWaveform) {
        updateWaveform = false;
        updateWaveformDisplay();
    }

    setTimeout(function () { update(webSocket); }, 200);
}

function sanitize(width) {
    zoom = Math.max(1, zoom);

    if (lastBarIdToShow != -1) {
        var barsToShow = Math.floor(zoom * width);
        lastBarIdToShow = Math.min(lastBarIdToShow, bars[bars.length - 1].id);

        if (bars.length >= barsToShow) {
            lastBarIdToShow = Math.max(lastBarIdToShow, bars[barsToShow].id);
        }
    }
}

function updateWaveformDisplay() {
    var c = document.getElementById("bars") as HTMLCanvasElement;
    var ctx = c.getContext("2d");
    var center = c.height / 2;

    c.width = c.clientWidth;
    c.height = c.clientHeight;

    ctx.clearRect(0, 0, c.width, c.height);
    ctx.strokeStyle = "#FFFFFF";
    ctx.lineWidth = 1;

    // middle line
    ctx.beginPath();
    ctx.moveTo(0, center);
    ctx.lineTo(c.clientWidth, center);
    ctx.stroke();

    if (bars.length == 0)
        return;

    sanitize(c.width);

    var lastId = lastBarIdToShow != -1 ? lastBarIdToShow : bars[bars.length - 1].id;
    var firstBarId = Math.max(0, lastId - c.width * zoom);
    firstBarId = Math.max(firstBarId, bars[0].id);
    var idx = firstBarId - bars[0].id;

    for (var i = 0; i < c.width; i++) {
        var m = 0;

        for (var q = 0; q < zoom; q++) {
            if (idx >= bars.length)
                break;

            m = Math.max(m, bars[idx++].max);
        }

        var m = center * m / 256;
        var top = center - m;
        var bottom = center + m;
        ctx.moveTo(i, top);
        ctx.lineTo(i, bottom);

        if (idx >= bars.length)
            break;
    }

    ctx.stroke();

    drawGrid(ctx, c.clientWidth, c.clientHeight, firstBarId - bars[0].id);

    var c = document.getElementById("selected-range") as HTMLCanvasElement;
    var left = (playbackRange.begin - firstBarId) / zoom;
    var right = (playbackRange.end - firstBarId) / zoom;
    var width = right - left;
    c.style.left = left + "px";
    c.style.width = width + "px";
}

function drawGrid(ctx, width, height, idx) {
    if (bars.length < 2)
        return;

    var pixelsPerTimeMarker = 100;
    var timeMarkersOnScreen = Math.floor(width / pixelsPerTimeMarker);
    var barsOnScreen = width * zoom;
    var barsPerTimeMarker = barsOnScreen / timeMarkersOnScreen;


    var fontHeight = 10;
    ctx.font = fontHeight + "px serif";

    for (var i = 0; i < width; i++) {

        if (idx >= bars.length)
            break;

        var serverTime = bars[idx].recordTime;

        for (var z = 0; z < zoom; z++) {
            idx++;
            if (idx % barsPerTimeMarker == 0) {
                ctx.moveTo(i, 0);
                ctx.lineTo(i, height - fontHeight);
                var str = buildTime(serverTime);
                var xOffset = ctx.measureText(str).width / 2;
                ctx.strokeText(str, i - xOffset, height - 2);
            }
        }
    }
    ctx.stroke();
}

function buildTime(serverTime) {
    var t = serverTime - timingInfo.serverTime + timingInfo.localTime;
    var d = new Date(t / 1000 / 1000);
    return d.getHours().toString().padStart(2, '0') + ":" + d.getMinutes().toString().padStart(2, '0') + ":" + d.getSeconds().toString().padStart(2, '0');
}

function update(webSocket) {
    messageHandler = (e) => onGotInfo(e, webSocket);
    webSocket.send(JSON.stringify({ "get-info": {} }));
}

window.onload = function () {
    var webSocket = new WebSocket("ws://localhost:8889");

    webSocket.onopen = function (event) {
        update(webSocket);
    };

    webSocket.onclose = function (event) {
        console.log("web socket is closed")
    };

    webSocket.onmessage = function (event) {
        var reader = new FileReader()
        reader.onload = () => {
            messageHandler(JSON.parse(reader.result as string));
        }
        reader.readAsText(event.data);
    }
}

function zoomIn() {
    zoom /= 2;
    updateWaveformDisplay();
}

function zoomOut() {
    zoom *= 2;
    updateWaveformDisplay();
}

function startDrag(e) {
    document.getElementById("waveform").setPointerCapture(e.pointerId);
    scrollX = e.screenX;
    dragging = Dragging.dragPane;
}

function stopDrag(e) {
    dragging = Dragging.noDrag;
    document.getElementById("waveform").releasePointerCapture(e.pointerId);
}

function drag(e) {
    if (dragging == Dragging.dragPane) {
        var diff = e.screenX - scrollX;
        scrollX = e.screenX;

        var lastId = bars[bars.length - 1].id;

        if (lastBarIdToShow == -1) {
            if (diff > 0) {
                lastBarIdToShow = lastId - diff * zoom;
            }
        }
        else {
            lastBarIdToShow = lastBarIdToShow - diff * zoom;
        }

        if (lastBarIdToShow > lastId - 5 * zoom)
            lastBarIdToShow = -1; // auto scroll

        updateWaveformDisplay();
    }
}

function fireAndForget(msg) {
    var webSocket = new WebSocket("ws://localhost:8889");

    webSocket.onopen = function (event) {
        webSocket.send(JSON.stringify(msg));
    };
}

function toggleRecording() {
    fireAndForget({ "toggle-recording": {} });
}

function pixToBar(pix) {
    var c = document.getElementById("bars");
    var lastId = lastBarIdToShow != -1 ? lastBarIdToShow : bars[bars.length - 1].id;
    var firstBarId = Math.max(bars[0].id, lastId - c.clientWidth * zoom);
    return firstBarId + pix * zoom;
}

function barToPix(pix) {
    var c = document.getElementById("bars");
    var lastId = lastBarIdToShow != -1 ? lastBarIdToShow : bars[bars.length - 1].id;
    var firstBarId = Math.max(bars[0].id, lastId - c.clientWidth * zoom);
    return firstBarId + pix * zoom;
}

function startDragRange(e) {
    document.getElementById("range-selector").setPointerCapture(e.pointerId);
    scrollX = e.screenX;
    dragging = Dragging.dragRange;
    playbackRange.begin = pixToBar(e.layerX);
    playbackRange.end = playbackRange.begin + 1;
    e.stopPropagation();
    updateWaveformDisplay();
}

function stopDragRange(e) {
    dragging = Dragging.noDrag;
    document.getElementById("range-selector").releasePointerCapture(e.pointerId);
}

function dragRange(e) {
    if (dragging == Dragging.dragRange) {
        playbackRange.end = pixToBar(e.layerX);
        e.stopPropagation();
        updateWaveformDisplay();
    }
}

function startPlayback(e) {
    fireAndForget({ "start-playback": { begin: playbackRange.begin, end: playbackRange.end } });
    e.stopPropagation();
    e.preventDefault();
}

function download(e) {
    e.stopPropagation();
    e.preventDefault();
}
