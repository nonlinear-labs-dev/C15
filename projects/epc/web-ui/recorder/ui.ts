class Draggable {
    constructor(id: string) {
        this.element = document.getElementById(id)!;
        this.element.onpointerdown = (e) => {
            this.dragging = true;
            this.element.setPointerCapture(e.pointerId);
            e.stopPropagation();
            e.preventDefault();
            this.startDrag(e);
        }

        this.element.onpointerup = (e) => {
            this.dragging = false;
            this.element.releasePointerCapture(e.pointerId);
            e.stopPropagation();
            e.preventDefault();
            this.stopDrag(e);
        }

        this.element.onpointermove = (e) => {
            if (this.dragging) {
                this.drag(e);
                e.stopPropagation();
            }
        }
    }

    startDrag(e: PointerEvent): void { }
    stopDrag(e: PointerEvent): void { }
    drag(e: PointerEvent): void { }

    getElement(): HTMLElement {
        return this.element;
    }

    private element: HTMLElement;
    private dragging = false;
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

class RangeBorderHandler extends Draggable {
    constructor(id: string, private startCB: () => void, private dragCB: (x: number) => void) {
        super(id);
    }

    startDrag(e: PointerEvent) {
        this.startCB();
    }

    drag(e: PointerEvent) {
        var waveform = document.getElementById("waveform")!;
        var walker = this.getElement();
        var x = e.offsetX;

        while (walker != waveform) {
            x += walker.offsetLeft;
            walker = walker.parentElement!;
        }

        this.dragCB(x);
    }
}

class PlaybackRange {
    constructor(private waveForm: Waveform) {
    }

    min(): number {
        if (this.waveForm.bars.count() == 0)
            return 0;

        return Math.max(Math.min(this.one, this.other), this.waveForm.bars.first().id);
    }

    max(): number {
        if (this.waveForm.bars.count() == 0)
            return 0;

        return Math.min(Math.max(this.one, this.other), this.waveForm.bars.last().id);
    }

    one = 0;
    other = 0;
}

class SelectedRange extends Draggable {
    constructor(private waveform: Waveform) {
        super("range-selector");

        this.playbackRange = new PlaybackRange(waveform);

        document.getElementById("toggle-playback")!.onpointerdown = (e) => this.togglePlayback(e);
        document.getElementById("download")!.onpointerdown = (e) => this.download(e);

        this.oneHandle = new RangeBorderHandler("one-handle", () => { }, (x) => {
            this.playbackRange.one = Math.round(this.pixToBar(x));
            this.waveform.update();
        });

        this.otherHandle = new RangeBorderHandler("other-handle", () => { }, (x) => {
            this.playbackRange.other = Math.round(this.pixToBar(x));
            this.waveform.update();
        });
    }

    update(firstBarId: number): void {
        var c = document.getElementById("selected-range") as HTMLCanvasElement;
        var left = (this.playbackRange.min() - firstBarId) / this.waveform.zoom;
        var right = (this.playbackRange.max() - firstBarId) / this.waveform.zoom;
        var width = right - left;
        c.style.left = left + "px";
        c.style.width = width + "px";

        var onePos = (this.playbackRange.one < this.playbackRange.other) ? 0 : 100;
        var otherPos = (this.playbackRange.one > this.playbackRange.other) ? 0 : 100;
        document.getElementById("one-border")!.style.left = onePos + "%";
        document.getElementById("other-border")!.style.left = otherPos + "%";

        var bars = this.waveform.bars;
        if (bars.count() > 0) {
            var fromIdx = this.playbackRange.min() - bars.first().id;
            var toIdx = this.playbackRange.max() - bars.first().id;

            if (fromIdx >= 0 && fromIdx < bars.count() && toIdx >= 0 && toIdx < bars.count()) {
                document.getElementById("selected-range-time-from")!.textContent = buildTime(
                    bars.get(fromIdx)!.recordTime, this.waveform.timingInfo);

                document.getElementById("selected-range-time-to")!.textContent = buildTime(
                    bars.get(toIdx)!.recordTime, this.waveform.timingInfo);
            }
        }
    }

    private togglePlayback(e: Event) {
        var pb = document.getElementById("toggle-playback")!;

        if (pb.classList.contains("paused"))
            fireAndForget({ "start-playback": { begin: this.playbackRange.min(), end: this.playbackRange.max() } });
        else
            fireAndForget({ "pause-playback": {} });

        e.stopPropagation();
        e.preventDefault();
    }

    private download(e: Event) {
        e.stopPropagation();
        e.preventDefault();
        var url = "http://" + hostName + httpPort + "/?begin=" + this.playbackRange.min() + "&end=" + this.playbackRange.max();
        window.location.assign(url);
    }

    private pixToBar(pix: number) {
        if (this.waveform.bars.count() == 0)
            return -1;

        var c = document.getElementById("bars") as HTMLDivElement;
        var lastId = this.waveform.lastBarIdToShow != -1 ? this.waveform.lastBarIdToShow : this.waveform.bars.last().id;
        var firstBarId = lastId - c.clientWidth * this.waveform.zoom;
        return firstBarId + pix * this.waveform.zoom;
    }

    startDrag(e: PointerEvent) {
        this.playbackRange.one = this.playbackRange.other = Math.round(this.pixToBar(e.offsetX));
        this.sanitize();
        this.waveform.update();
    }


    drag(e: PointerEvent) {
        this.playbackRange.other = Math.round(this.pixToBar(e.offsetX));
        this.sanitize();
        this.waveform.update();
    }

    private sanitize() {
        if (this.waveform.bars.count() > 0) {
            this.playbackRange.one = Math.min(this.playbackRange.one, this.waveform.bars.last().id);
            this.playbackRange.other = Math.min(this.playbackRange.other, this.waveform.bars.last().id);
            this.playbackRange.one = Math.max(this.playbackRange.one, this.waveform.bars.first().id);
            this.playbackRange.other = Math.max(this.playbackRange.other, this.waveform.bars.first().id);
        }
    }

    private playbackRange: PlaybackRange;
    private oneHandle: RangeBorderHandler;
    private otherHandle: RangeBorderHandler;
}

class ScrollbarHandle extends Draggable {
    constructor(id: string, private cb: (x: number) => void) {
        super(id);
    }

    startDrag(e: PointerEvent) {
        this.x = e.screenX;
    }

    drag(e: PointerEvent) {
        this.cb(e.screenX - this.x);
        this.x = e.screenX;
    }

    private x = 0;

}

class Scrollbar extends Draggable {
    private leftHandle: ScrollbarHandle;
    private rightHandle: ScrollbarHandle;

    constructor(private waveform: Waveform) {
        super("scrollbar-handle");

        this.rightHandle = new ScrollbarHandle("scrollbar-handle-right", (x: number) => {
            var first = this.waveform.getFirstVisibleBar();
            var last = this.waveform.getLastVisibleBar();
            var c = document.getElementById("bars") as HTMLCanvasElement;
            var handle = document.getElementById("scrollbar-handle")!;
            var w = Number.parseFloat(handle.style.width!) / 100;
            this.waveform.showBars(first, last + x * this.waveform.zoom / w);
        });

        this.leftHandle = new ScrollbarHandle("scrollbar-handle-left", (x: number) => {
            var first = this.waveform.getFirstVisibleBar();
            var last = this.waveform.getLastVisibleBar();
            var c = document.getElementById("bars") as HTMLCanvasElement;
            var handle = document.getElementById("scrollbar-handle")!;
            var w = Number.parseFloat(handle.style.width!) / 100;
            this.waveform.showBars(first + x * this.waveform.zoom / w, last);
        });

    }

    update(firstBarId: number): void {
        var c = document.getElementById("bars") as HTMLCanvasElement;
        var width = c.width;
        var numBars = this.waveform.bars.count();
        var barsToShow = numBars / this.waveform.zoom;
        var e = document.getElementById("scrollbar")!;
        e.style.visibility = (width > barsToShow) ? "hidden" : "visible";

        var handle = document.getElementById("scrollbar-handle")!;
        var handleWidth = 100 * width / barsToShow;
        handle.style.width = handleWidth + "%";
        var handleWidthPX = handle.getBoundingClientRect().width;
        var left = (width - handleWidthPX) * firstBarId / (numBars - width * this.waveform.zoom);
        handle.style.left = left + "px";
    }

    startDrag(e: PointerEvent) {
        this.x = e.screenX;
    }

    drag(e: PointerEvent) {
        var handle = document.getElementById("scrollbar-handle")!;
        var w = Number.parseFloat(handle.style.width!) / 100;
        this.waveform.scrollBy((this.x - e.screenX) / w);
        this.x = e.screenX;
    }

    private x = 0;
}

class Waveform extends Draggable {
    constructor(public bars: Bars, public timingInfo: TimingInfo) {
        super("waveform");
        this.selectedRange = new SelectedRange(this);
        this.scrollbar = new Scrollbar(this);

        var waveForm = document.getElementById("waveform")!;
        waveForm.onwheel = (e) => this.wheel(e);

        document.getElementById("zoom-in")!.onclick = (e) => this.zoomIn();
        document.getElementById("zoom-out")!.onclick = (e) => this.zoomOut();
    }

    update() {
        var c = document.getElementById("bars") as HTMLCanvasElement;
        var ctx = c.getContext("2d") as CanvasRenderingContext2D;

        c.width = c.clientWidth;
        c.height = c.clientHeight;

        var center = c.height / 2;

        ctx.clearRect(0, 0, c.width, c.height);
        ctx.strokeStyle = window.getComputedStyle(document.querySelector('#bars')!).color!;
        ctx.lineWidth = 1;

        // middle line
        ctx.beginPath();
        ctx.moveTo(0, center);
        ctx.lineTo(c.clientWidth, center);
        ctx.stroke();

        if (this.bars.count() == 0)
            return;

        this.sanitize(c.width);

        var lastId = this.lastBarIdToShow != -1 ? this.lastBarIdToShow : this.bars.last().id;
        var firstBarId = lastId - c.width * this.zoom;
        var id = firstBarId;

        for (var i = 0; i < c.width; i++) {
            var m = this.bars.getMax(id, this.zoom);

            var from = Math.round(id);
            var to = Math.round(id + this.zoom);
            id += this.zoom;

            var m = center * m / 256;
            var top = center - m;
            var bottom = center + m;
            ctx.moveTo(i, top);
            ctx.lineTo(i, bottom);

            if (id >= lastId)
                break;
        }

        ctx.stroke();
        this.drawGrid(ctx, c.clientWidth, c.clientHeight, firstBarId - this.bars.first().id);
        this.selectedRange.update(firstBarId);
        this.scrollbar.update(firstBarId);

        if (this.bars.count() > 0) {
            var playPosIndicator = document.getElementById("play-pos") as HTMLElement;
            var lastId = this.lastBarIdToShow != -1 ? this.lastBarIdToShow : this.bars.last().id;
            var firstBarId = lastId - c.width * this.zoom;
            playPosIndicator.style.left = (this.playPos - firstBarId) / this.zoom + "px";
            playPosIndicator.style.visibility = this.playPos >= 0 ? "visible" : "hidden";
        }
    }

    private drawGrid(ctx: CanvasRenderingContext2D, width: number, height: number, idx: number) {
        if (this.bars.count() < 2)
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

        var serverTime = this.bars.first().recordTime;

        for (var i = 0; i < width; i++) {
            var from = Math.round(idx);
            var to = Math.round(idx + this.zoom);

            var isWrapInRange = (from % barsPerTimeMarker) > (to % barsPerTimeMarker);
            idx += this.zoom;

            if (isWrapInRange) {
                var b = this.bars.get(from) || this.bars.get(to);
                serverTime = b ? b.recordTime : serverTime + nsPerTimeMarker;
                ctx.moveTo(i, 0);
                ctx.lineTo(i, height - fontHeight);
                var str = buildTime(serverTime, this.timingInfo);
                var xOffset = ctx.measureText(str).width / 2;
                ctx.strokeText(str, i - xOffset, height - 2);
            }
        }
        ctx.stroke();
    }

    getFirstVisibleBar(): number {
        if (this.bars.count() == 0)
            return -1;

        var c = document.getElementById("bars") as HTMLCanvasElement;
        return this.getLastVisibleBar() - c.clientWidth * this.zoom;
    }

    getLastVisibleBar(): number {
        if (this.bars.count() == 0)
            return -1;
        return this.lastBarIdToShow != -1 ? this.lastBarIdToShow : this.bars.last().id;
    }

    showBars(first: number, last: number) {
        var c = document.getElementById("bars") as HTMLCanvasElement;
        var numBars = last - first;
        var newZoom = Math.max(1, numBars / c.width);

        if (newZoom == this.zoom)
            return;

        this.zoom = newZoom;
        this.lastBarIdToShow = last;
        this.update();
    }

    private wheel(e: WheelEvent) {
        var oldZoom = this.zoom;
        var newZoom = oldZoom + (e.deltaY < 0 ? 1 : -1);
        newZoom = Math.max(newZoom, 1);

        if (newZoom == oldZoom)
            return;

        var c = document.getElementById("bars") as HTMLDivElement;
        var lastId = this.lastBarIdToShow != -1 ? this.lastBarIdToShow : this.bars.last().id;
        var firstBarId = lastId - c.clientWidth * oldZoom;
        var wheelOnBar = firstBarId + e.offsetX * oldZoom;
        var b = wheelOnBar + newZoom * c.clientWidth - e.offsetX * newZoom;

        this.lastBarIdToShow = b;
        this.zoom = newZoom;
        this.update();
    }

    startDrag(e: PointerEvent) {
        this.scrollX = e.screenX;
        this.pointers.push(e);

        if (this.pointers.length == 2) {
            var xDiff = this.pointers[0].clientX - this.pointers[1].clientX;
            this.barsPerPixelOnPinchStart = xDiff * this.zoom;
        }
    }

    stopDrag(e: PointerEvent) {
        for (var i = 0; i < this.pointers.length; i++) {
            if (this.pointers[i].pointerId == e.pointerId) {
                this.pointers.splice(i, 1);
                break;
            }
        }
    }

    drag(e: PointerEvent) {
        for (var i = 0; i < this.pointers.length; i++) {
            if (e.pointerId == this.pointers[i].pointerId) {
                this.pointers[i] = e;
                break;
            }
        }

        if (this.pointers.length == 1)
            this.doScroll(e);
        else if (this.pointers.length == 2)
            this.doPinch();
    }

    scrollBy(amount: number) {
        var lastId = this.bars.last().id;

        if (this.lastBarIdToShow == -1) {
            if (amount > 0)
                this.lastBarIdToShow = Math.round(lastId - amount * this.zoom);
        }
        else {
            this.lastBarIdToShow = Math.round(this.lastBarIdToShow - amount * this.zoom);
        }

        if (this.lastBarIdToShow > lastId - 5 * this.zoom)
            this.lastBarIdToShow = -1; // auto scroll

        this.update();
    }


    private doScroll(e: PointerEvent) {
        this.scrollBy(e.screenX - this.scrollX);
        this.scrollX = Math.round(e.screenX);
    }

    private doPinch() {
        var xDiff = this.pointers[0].clientX - this.pointers[1].clientX;
        var X = this.pointers[0].clientX + xDiff / 2;

        var oldZoom = this.zoom;
        var newZoom = Math.max(1, this.barsPerPixelOnPinchStart / xDiff);

        if (oldZoom != newZoom) {
            var c = document.getElementById("bars") as HTMLDivElement;
            var lastId = this.lastBarIdToShow != -1 ? this.lastBarIdToShow : this.bars.last().id;
            var firstBarId = lastId - c.clientWidth * oldZoom;
            var wheelOnBar = firstBarId + X * oldZoom;
            var b = wheelOnBar + newZoom * c.clientWidth - X * newZoom;

            this.lastBarIdToShow = b;
            this.zoom = newZoom;
        }
        this.update();

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

    private sanitize(width: number) {
        this.zoom = Math.max(1, this.zoom);

        var c = document.getElementById("bars") as HTMLCanvasElement;

        if (c.width * this.zoom > this.bars.count())
            this.lastBarIdToShow = -1; // auto scroll

        if (this.lastBarIdToShow != -1) {
            var barsToShow = Math.floor(this.zoom * width);
            this.lastBarIdToShow = Math.min(this.lastBarIdToShow, this.bars.last().id);

            if (this.bars.count() > barsToShow) {
                this.lastBarIdToShow = Math.max(this.lastBarIdToShow, this.bars.get(barsToShow)!.id);
            }
        }
    }

    zoom = 1.0;
    lastBarIdToShow = -1; // -1 = autoscroll

    private scrollX = -1;
    private selectedRange: SelectedRange;
    private playPos = -1;

    private pointers = new Array<PointerEvent>();
    private barsPerPixelOnPinchStart = 0;

    private scrollbar: Scrollbar;
}

class UI {
    constructor(bars: Bars, private timingInfo: TimingInfo) {
        this.waveform = new Waveform(bars, timingInfo);
        document.getElementById("toggle-recording")!.onclick = (e) => this.toggleRecording();
        document.getElementById("reset")!.onclick = (e) => this.reset();
    }

    updateFirstAndLastFrame(firstTime: number, lastTime: number) {
        document.getElementById("first-available-frame-timestamp")!.textContent = buildTime(firstTime, this.timingInfo);
        document.getElementById("last-available-frame-timestamp")!.textContent = buildTime(lastTime, this.timingInfo);
    }

    updateMemoryUsage(current: number, max: number) {
        document.getElementById("consumed-mem")!.textContent = this.formatMemSize(current);
        document.getElementById("max-mem")!.textContent = this.formatMemSize(max);
    }

    updateTransportStates(recorderPaused: boolean, playPaused: boolean, playPos: number) {
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

    update() {
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

    private updateStream: UpdateStream | null = null;
    private waveform: Waveform;
}
