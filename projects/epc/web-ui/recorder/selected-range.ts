class SelectedRange extends Draggable {
    constructor(private c15: C15ProxyIface, private waveform: Waveform) {
        super("range-selector");

        this.playbackRange = new PlaybackRange(c15);

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
        var range = document.getElementById("selected-range")!;
        var download = document.getElementById("download")!;

        if (this.playbackRange.empty()) {
            range.style.visibility = "hidden";
            download.classList.add("disabled");
        }
        else {
            range.style.visibility = "visible";
            download.classList.remove("disabled");
        }

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

        this.oneHandle.setTime("");
        this.otherHandle.setTime("");

        var bars = this.c15.getBars();
        if (bars.count() > 0) {
            var fromId = this.playbackRange.min();
            var toId = this.playbackRange.max();
            var bar = bars.get(fromId);
            if (bar) {
                var fromTime = this.c15.buildTime(bar.recordTime);
                document.getElementById("selected-range-time-from")!.textContent = fromTime;

                var toTime = this.c15.buildTime(bars.get(toId)!.recordTime);
                document.getElementById("selected-range-time-to")!.textContent = toTime;

                this.oneHandle.setTime(this.playbackRange.one < this.playbackRange.other ? fromTime : toTime);
                this.otherHandle.setTime(this.playbackRange.one < this.playbackRange.other ? toTime : fromTime);
            }
        }
    }

    private pixToBar(pix: number) {
        if (this.c15.getBars().count() == 0)
            return -1;

        var c = document.getElementById("bars") as HTMLDivElement;
        var lastId = this.waveform.lastBarIdToShow != -1 ? this.waveform.lastBarIdToShow : this.c15.getBars().last().id;
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
        if (this.c15.getBars().count() > 0) {
            this.playbackRange.one = Math.min(this.playbackRange.one, this.c15.getBars().last().id);
            this.playbackRange.other = Math.min(this.playbackRange.other, this.c15.getBars().last().id);
            this.playbackRange.one = Math.max(this.playbackRange.one, this.c15.getBars().first().id);
            this.playbackRange.other = Math.max(this.playbackRange.other, this.c15.getBars().first().id);
        }
    }

    public playbackRange: PlaybackRange;
    private oneHandle: RangeBorderHandler;
    private otherHandle: RangeBorderHandler;
}