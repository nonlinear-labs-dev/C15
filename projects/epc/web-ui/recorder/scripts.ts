const httpPort = ":8890";
const wsPort = ":8889";
const hostName = "192.168.8.2";

class TimingInfo {
    serverTime: number = 0;
    localTime: number = 0;
}

class Bar {
    id: number = 0;
    max: number = 0;
    recordTime: number = 0;
}

class UpdateStream {
    constructor() {
    }

    public connect(display: UI): void {
        console.log("connect");
        this.ui = display;
        this.bars.splice(0);
        this.socket = new WebSocket("ws://" + hostName + wsPort);
        this.socket.onopen = (event) => this.update();
        this.socket.onerror = (event) => this.retry();
        this.socket.onclose = (event) => this.retry();
        this.socket.onmessage = (event) => this.readMessage(event.data);
    }

    private update(): void {
        console.log("update");
        this.messageHandler = (e) => this.processInfo(e);

        try {
            this.socket!.send(JSON.stringify({ "get-info": {} }));
        }
        catch (err) {
            this.retry();
        }

    }

    private processInfo(info: any) {
        this.timingInfo.serverTime = info.time;
        this.timingInfo.localTime = Date.now() * 1000 * 1000;

        if (!info.storage) {
            this.scheduleUpdate();
            return;
        }

        this.ui!.updateFirstAndLastFrame(info.storage.first.recordTime, info.storage.last.recordTime);
        this.ui!.updateMemoryUsage(info.storage.memUsage.current, info.storage.memUsage.max);
        this.ui!.updateTransportStates(info.recorder.paused, info.player.paused, info.player.pos);

        // remove outdated bars
        while (this.bars.length > 0) {
            if (this.bars[0].id >= info.storage.first.id)
                break;

            this.bars.shift();
            this.updateWaveform = true;
        }

        // query new bars
        var nextId = info.storage.first.id;

        if (this.bars.length > 0)
            nextId = this.bars[this.bars.length - 1].id + 1;

        if (info.storage.last.id - nextId < 1) {
            this.scheduleUpdate();
            return;
        }

        this.messageHandler = (e) => this.processQuery(e);
        this.socket!.send(JSON.stringify({ "query-frames": { "begin": nextId, "end": info.storage.last.id } }));
    }

    private scheduleUpdate() {
        if (this.updateTimer == -1) {
            console.log("schedule update");
            this.updateTimer = setTimeout(() => {
                this.updateTimer = -1;
                this.update();
            }, 200);
        }
    }

    private processQuery(info: any) {
        info.forEach((a: Bar) => {
            this.bars.push(a);
            this.updateWaveform = true;
        });

        if (this.updateWaveform) {
            this.updateWaveform = false;
            this.ui!.update();
        }

        this.scheduleUpdate();
    }

    private retry(): void {
        if (this.retryTimer == -1) {
            console.log("retry connection");
            this.retryTimer = setTimeout(() => {
                this.retryTimer = -1;
                this.connect(this.ui!);
            }, 200);
        }
    }

    private readMessage(data: any): void {
        var reader = new FileReader()
        reader.onload = () => {
            this.messageHandler!(JSON.parse(reader.result as string));
        }
        reader.readAsText(data);
    }


    private socket: WebSocket | null = null;
    private ui: UI | null = null;
    private messageHandler: ((e: any) => void) | null = null;

    public bars: Array<Bar> = new Array<Bar>();
    public timingInfo: TimingInfo = new TimingInfo();

    private updateWaveform = false;
    private retryTimer = -1;
    private updateTimer = -1;
}

enum Dragging {
    noDrag,
    dragPane,
    dragLeftBorder,
    dragRightBorder,
    dragRange
}

function buildTime(serverTime: number, timingInfo: TimingInfo): string {
    var t = serverTime - timingInfo.serverTime + timingInfo.localTime;
    var d = new Date(t / 1000 / 1000);
    return d.getHours().toString().padStart(2, '0') + ":" + d.getMinutes().toString().padStart(2, '0') + ":" + d.getSeconds().toString().padStart(2, '0');
}

function fireAndForget(msg: Object) {
    var webSocket = new WebSocket("ws://" + hostName + wsPort);

    webSocket.onopen = function (event) {
        webSocket.send(JSON.stringify(msg));
    };
}


class SelectedRange {
    constructor(private waveform: Waveform) {
        document.getElementById("toggle-playback")!.onpointerdown = (e) => this.togglePlayback(e);
        document.getElementById("download")!.onpointerdown = (e) => this.download(e);

        var rangeSelector = document.getElementById("range-selector")!;
        rangeSelector.onpointerdown = (e) => this.startDragRange(e);
        rangeSelector.onpointerup = (e) => this.stopDragRange(e);
        rangeSelector.onpointermove = (e) => this.dragRange(e);
    }

    public update(firstBarId: number) {
        var c = document.getElementById("selected-range") as HTMLCanvasElement;
        var left = (this.playbackRange.begin - firstBarId) / this.waveform.zoom;
        var right = (this.playbackRange.end - firstBarId) / this.waveform.zoom;
        var width = right - left;
        c.style.left = left + "px";
        c.style.width = width + "px";

        var bars = this.waveform.bars;
        if (bars.length > 0) {
            var fromIdx = this.playbackRange.begin - bars[0].id;
            var toIdx = this.playbackRange.end - bars[0].id;

            if (fromIdx >= 0 && fromIdx < bars.length && toIdx >= 0 && toIdx < bars.length) {
                document.getElementById("selected-range-time-from")!.textContent = buildTime(
                    bars[fromIdx].recordTime, this.waveform.timingInfo);

                document.getElementById("selected-range-time-to")!.textContent = buildTime(
                    bars[toIdx].recordTime, this.waveform.timingInfo);
            }
        }
    }

    private togglePlayback(e: Event) {
        var pb = document.getElementById("toggle-playback")!;

        if (pb.classList.contains("paused"))
            fireAndForget({ "start-playback": { begin: this.playbackRange.begin, end: this.playbackRange.end } });
        else
            fireAndForget({ "pause-playback": {} });

        e.stopPropagation();
        e.preventDefault();
    }

    private download(e: Event) {
        e.stopPropagation();
        e.preventDefault();
        var url = "http://" + hostName + httpPort + "/?begin=" + this.playbackRange.begin + "&end=" + this.playbackRange.end;
        window.location.assign(url);
    }

    private pixToBar(pix: number) {
        if (this.waveform.bars.length == 0)
            return -1;

        var c = document.getElementById("bars") as HTMLDivElement;
        var lastId = this.waveform.lastBarIdToShow != -1 ? this.waveform.lastBarIdToShow : this.waveform.bars[this.waveform.bars.length - 1].id;
        var firstBarId = lastId - c.clientWidth * this.waveform.zoom;
        return firstBarId + pix * this.waveform.zoom;
    }

    private startDragRange(e: PointerEvent) {
        document.getElementById("range-selector")!.setPointerCapture(e.pointerId);
        scrollX = e.screenX;
        this.dragging = true;
        this.playbackRange.begin = Math.round(this.pixToBar(e.offsetX));
        this.playbackRange.end = this.playbackRange.begin + 1;
        this.sanitize();
        e.stopPropagation();
        this.waveform.update();
    }

    private stopDragRange(e: PointerEvent) {
        this.dragging = false;
        document.getElementById("range-selector")!.releasePointerCapture(e.pointerId);
    }

    private dragRange(e: PointerEvent) {
        if (this.dragging) {
            this.playbackRange.end = Math.round(this.pixToBar(e.offsetX));
            this.sanitize();
            e.stopPropagation();
            this.waveform.update();
        }
    }

    private sanitize() {
        if (this.waveform.bars.length > 0) {
            this.playbackRange.begin = Math.min(this.playbackRange.begin, this.waveform.bars[this.waveform.bars.length - 1].id);
            this.playbackRange.end = Math.min(this.playbackRange.end, this.waveform.bars[this.waveform.bars.length - 1].id);
            this.playbackRange.begin = Math.max(this.playbackRange.begin, this.waveform.bars[0].id);
            this.playbackRange.end = Math.max(this.playbackRange.end, this.waveform.bars[0].id);
        }
    }

    private playbackRange = {
        begin: 0, end: 0
    }
    private dragging = false;
}


class Waveform {
    constructor(public bars: Array<any>, public timingInfo: TimingInfo) {
        this.selectedRange = new SelectedRange(this);

        var waveForm = document.getElementById("waveform")!;
        waveForm.onpointerdown = (e) => this.startDrag(e);
        waveForm.onpointerup = (e) => this.stopDrag(e);
        waveForm.onpointermove = (e) => this.drag(e);

        document.getElementById("zoom-in")!.onclick = (e) => this.zoomIn();
        document.getElementById("zoom-out")!.onclick = (e) => this.zoomOut();
    }

    public update() {
        var c = document.getElementById("bars") as HTMLCanvasElement;
        var ctx = c.getContext("2d") as CanvasRenderingContext2D;

        c.width = c.clientWidth;
        c.height = c.clientHeight;

        var center = c.height / 2;

        ctx.clearRect(0, 0, c.width, c.height);
        ctx.strokeStyle = "#FFFFFF";
        ctx.lineWidth = 1;

        // middle line
        ctx.beginPath();
        ctx.moveTo(0, center);
        ctx.lineTo(c.clientWidth, center);
        ctx.stroke();

        if (this.bars.length == 0)
            return;

        this.sanitize(c.width);

        document.getElementById("current-zoom")!.textContent = this.zoom + "x";

        var lastId = this.lastBarIdToShow != -1 ? this.lastBarIdToShow : this.bars[this.bars.length - 1].id;
        var firstBarId = lastId - c.width * this.zoom;
        var idx = firstBarId - this.bars[0].id;

        for (var i = 0; i < c.width; i++) {
            var m = 0;

            for (var q = 0; q < this.zoom; q++) {
                if (idx >= this.bars.length || idx < 0)
                    break;

                m = Math.max(m, this.bars[idx++].max);
            }

            var m = center * m / 256;
            var top = center - m;
            var bottom = center + m;
            ctx.moveTo(i, top);
            ctx.lineTo(i, bottom);

            if (idx >= this.bars.length)
                break;
        }

        ctx.stroke();
        this.drawGrid(ctx, c.clientWidth, c.clientHeight, firstBarId - this.bars[0].id);
        this.selectedRange.update(firstBarId);

        if (this.bars.length > 0) {
            var playPosIndicator = document.getElementById("play-pos") as HTMLElement;
            var lastId = this.lastBarIdToShow != -1 ? this.lastBarIdToShow : this.bars[this.bars.length - 1].id;
            var firstBarId = lastId - c.width * this.zoom;
            playPosIndicator.style.left = (this.playPos - firstBarId) / this.zoom + "px";
            playPosIndicator.style.visibility = this.playPos >= 0 ? "visible" : "hidden";
        }
    }

    private drawGrid(ctx: CanvasRenderingContext2D, width: number, height: number, idx: number) {
        if (this.bars.length < 2)
            return;

        var pixelsPerTimeMarker = 100;
        var timeMarkersOnScreen = Math.floor(width / pixelsPerTimeMarker);
        var barsOnScreen = width * this.zoom;
        var barsPerTimeMarker = Math.floor(barsOnScreen / timeMarkersOnScreen);

        var samplesPerBar = 4096; // constant defined in audio-engine
        var samplerate = 48000;

        var samplesPerTimeMarker = barsPerTimeMarker * samplesPerBar;
        var msPerTimeMarker = 1000 * samplesPerTimeMarker / samplerate;
        var nsPerTimeMarker = 1000 * 1000 * msPerTimeMarker;

        var fontHeight = 10;
        ctx.font = fontHeight + "px serif";

        var serverTime = this.bars[0].recordTime;

        for (var i = 0; i < width; i++) {
            for (var z = 0; z < this.zoom; z++) {
                idx++;
                if (idx % barsPerTimeMarker == 0 && idx >= 0) {
                    serverTime = idx < this.bars.length && idx >= 0 ? this.bars[idx].recordTime : serverTime + nsPerTimeMarker;

                    ctx.moveTo(i, 0);
                    ctx.lineTo(i, height - fontHeight);
                    var str = buildTime(serverTime, this.timingInfo);
                    var xOffset = ctx.measureText(str).width / 2;
                    ctx.strokeText(str, i - xOffset, height - 2);
                }
            }
        }
        ctx.stroke();
    }

    private startDrag(e: PointerEvent) {
        document.getElementById("waveform")!.setPointerCapture(e.pointerId);
        scrollX = e.screenX;
        this.dragging = true;
    }

    private stopDrag(e: PointerEvent) {
        this.dragging = false;
        document.getElementById("waveform")!.releasePointerCapture(e.pointerId);
    }

    private drag(e: PointerEvent) {
        if (this.dragging) {
            var diff = e.screenX - scrollX;
            scrollX = e.screenX;

            var lastId = this.bars[this.bars.length - 1].id;

            if (this.lastBarIdToShow == -1) {
                if (diff > 0) {
                    this.lastBarIdToShow = lastId - diff * this.zoom;
                }
            }
            else {
                this.lastBarIdToShow = this.lastBarIdToShow - diff * this.zoom;
            }

            if (this.lastBarIdToShow > lastId - 5 * this.zoom)
                this.lastBarIdToShow = -1; // auto scroll

            this.update();
        }
    }

    setPlayPos(pos: number) {
        if (this.playPos != pos) {
            this.playPos = pos;
            this.update();
        }
    }

    private zoomIn() {
        this.zoom /= 2;
        this.update();
    }

    private zoomOut() {
        this.zoom *= 2;
        this.update();
    }

    private sanitize(width: any) {
        this.zoom = Math.max(1, this.zoom);

        if (this.lastBarIdToShow != -1) {
            var barsToShow = Math.floor(this.zoom * width);
            this.lastBarIdToShow = Math.min(this.lastBarIdToShow, this.bars[this.bars.length - 1].id);

            if (this.bars.length > barsToShow) {
                this.lastBarIdToShow = Math.max(this.lastBarIdToShow, this.bars[barsToShow].id);
            }
        }
    }

    public zoom = 1.0;
    public lastBarIdToShow = -1; // -1 = autoscroll

    private dragging = false;
    private scrollX = -1;
    private selectedRange: SelectedRange;
    private playPos = -1;
}

class UI {
    constructor(bars: Array<any>, private timingInfo: TimingInfo) {
        this.waveform = new Waveform(bars, timingInfo);
        document.getElementById("toggle-recording")!.onclick = (e) => this.toggleRecording();
        document.getElementById("reset")!.onclick = (e) => this.reset();
    }

    public updateFirstAndLastFrame(firstTime: number, lastTime: number) {
        document.getElementById("first-available-frame-timestamp")!.textContent = buildTime(firstTime, this.timingInfo);
        document.getElementById("last-available-frame-timestamp")!.textContent = buildTime(lastTime, this.timingInfo);
    }

    public updateMemoryUsage(current: number, max: number) {
        document.getElementById("consumed-mem")!.textContent = this.formatMemSize(current);
        document.getElementById("max-mem")!.textContent = this.formatMemSize(max);
    }

    public updateTransportStates(recorderPaused: boolean, playPaused: boolean, playPos: number) {
        var rec = document.getElementById("toggle-recording")!;
        var pb = document.getElementById("toggle-playback")!;

        this.waveform.setPlayPos(playPos);

        if (recorderPaused) {
            rec.classList.remove("recording");
            rec.classList.add("paused");
        }
        else {
            rec.classList.add("recording");
            rec.classList.remove("paused");
        }

        if (playPaused) {
            pb.classList.remove("playing");
            pb.classList.add("paused");
        }
        else {
            pb.classList.add("playing");
            pb.classList.remove("paused");
        }
    }

    public update() {
        this.waveform.update();
    }

    private formatMemSize(s: number) {
        var i = 0;
        var units = [' B', ' kB', ' MB'];

        while (s > 1024 && i < 2) {
            s = s / 1024;
            i++;
        }

        return Math.max(s, 0.1).toFixed(1) + units[i];
    }

    private reset() {
        fireAndForget({ "reset": {} });
    }

    private toggleRecording() {
        fireAndForget({ "toggle-recording": {} });
    }

    private dragging = Dragging.noDrag;
    private updateStream: UpdateStream | null = null;
    private waveform: Waveform;
}


window.onload = function () {
    var updateStream = new UpdateStream();
    var display = new UI(updateStream.bars, updateStream.timingInfo);
    updateStream.connect(display);
}

