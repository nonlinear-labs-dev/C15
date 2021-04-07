class SelectedRange extends Draggable {
    constructor(private waveform: Waveform) {
        super("range-selector");

        this.playbackRange = new PlaybackRange(waveform);

        document.getElementById("download")!.onpointerdown = (e) => this.download(e);

        this.oneHandle = new RangeBorderHandler("one-handle", () => { }, (x) => {
            this.playbackRange.one = Math.round(this.pixToBar(x));
            this.waveform.update();
        });

        this.otherHandle = new RangeBorderHandler("other-handle", () => { }, (x) => {
            this.playbackRange.other = Math.round(this.pixToBar(x));
            this.waveform.update();
        });

        var undoOne = document.getElementById("undo-one")!;
        var undoOther = document.getElementById("undo-other")!;

        undoOne.onpointerdown = (e) => { e.stopPropagation(); };
        undoOther.onpointerdown = (e) => { e.stopPropagation(); };

        undoOne.onclick = (e) => {
            e.preventDefault();
            this.jumpUndo(this.playbackRange.one);
        };

        undoOther.onclick = (e) => {
            e.preventDefault();
            this.jumpUndo(this.playbackRange.other);
        };
    }

    jumpUndo(barId: number) {
        var bar = this.waveform.bars.get(barId);
        if (bar) {
            var r = new XMLHttpRequest();
            var url = "http://" + hostName + playgroundHttpPort + "/undo/jump-to-timestamp?timestamp=" + bar.recordTime;
            r.open("GET", url, true);
            r.send();
        }
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

        this.oneHandle.setTime("");
        this.otherHandle.setTime("");

        var bars = this.waveform.bars;
        if (bars.count() > 0) {
            var fromId = this.playbackRange.min();
            var toId = this.playbackRange.max();
            var fromTime = buildTime(bars.get(fromId)!.recordTime, this.waveform.timingInfo);
            document.getElementById("selected-range-time-from")!.textContent = fromTime;

            var toTime = buildTime(bars.get(toId)!.recordTime, this.waveform.timingInfo);
            document.getElementById("selected-range-time-to")!.textContent = toTime;

            this.oneHandle.setTime(this.playbackRange.one < this.playbackRange.other ? fromTime : toTime);
            this.otherHandle.setTime(this.playbackRange.one < this.playbackRange.other ? toTime : fromTime);
        }
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

    public playbackRange: PlaybackRange;
    private oneHandle: RangeBorderHandler;
    private otherHandle: RangeBorderHandler;
}