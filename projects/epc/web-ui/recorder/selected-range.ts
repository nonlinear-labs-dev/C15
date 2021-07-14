class SelectedRange extends Draggable {
    constructor(private c15: C15ProxyIface, private waveform: Waveform) {
        super("range-selector");

        this.barRange = new BarRange(c15);

        this.oneHandle = new RangeBorderHandler("one-handle", () => { }, (x) => {
            this.barRange.one = Math.round(this.pixToBar(x));
            this.waveform.update();
        });

        this.otherHandle = new RangeBorderHandler("other-handle", () => { }, (x) => {
            this.barRange.other = Math.round(this.pixToBar(x));
            this.waveform.update();
        });
    }

    update(firstBarId: number): void {
        var range = document.getElementById("selected-range")!;
        var download = document.getElementById("download")!;

        if (this.barRange.empty()) {
            range.style.visibility = "hidden";
            download.classList.add("disabled");
        }
        else {
            range.style.visibility = "visible";
            download.classList.remove("disabled");
        }

        var c = document.getElementById("selected-range") as HTMLCanvasElement;
        var left = (this.barRange.min() - firstBarId) / this.waveform.zoom;
        var right = (this.barRange.max() - firstBarId) / this.waveform.zoom;
        var width = right - left;
        c.style.left = left + "px";
        c.style.width = width + "px";

        var onePos = (this.barRange.one < this.barRange.other) ? 0 : 100;
        var otherPos = (this.barRange.one > this.barRange.other) ? 0 : 100;
        document.getElementById("one-border")!.style.left = onePos + "%";
        document.getElementById("other-border")!.style.left = otherPos + "%";

        this.oneHandle.setTime("");
        this.otherHandle.setTime("");

        var bars = this.c15.getBars();
        if (bars.count() > 0) {
            var fromId = this.barRange.min();
            var toId = this.barRange.max();
            var bar = bars.get(fromId);
            if (bar) {
                var fromTime = this.c15.buildTime(bar.recordTime);
                document.getElementById("selected-range-time-from")!.textContent = fromTime;

                var toTime = this.c15.buildTime(bars.get(toId)!.recordTime);
                document.getElementById("selected-range-time-to")!.textContent = toTime;

                this.oneHandle.setTime(this.barRange.one < this.barRange.other ? fromTime : toTime);
                this.otherHandle.setTime(this.barRange.one < this.barRange.other ? toTime : fromTime);
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
        this.barRange.one = this.barRange.other = Math.round(this.pixToBar(e.offsetX));
        this.sanitize();
        this.waveform.update();
    }


    drag(e: PointerEvent) {
        this.barRange.other = Math.round(this.pixToBar(e.offsetX));
        this.sanitize();
        this.waveform.update();
    }

    private sanitize() {
        if (this.c15.getBars().count() > 0) {
            this.barRange.one = Math.min(this.barRange.one, this.c15.getBars().last().id);
            this.barRange.other = Math.min(this.barRange.other, this.c15.getBars().last().id);
            this.barRange.one = Math.max(this.barRange.one, this.c15.getBars().first().id);
            this.barRange.other = Math.max(this.barRange.other, this.c15.getBars().first().id);
        }
    }

    public barRange: BarRange;
    private oneHandle: RangeBorderHandler;
    private otherHandle: RangeBorderHandler;
}