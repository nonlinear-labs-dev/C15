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
        var left = (width - handleWidthPX) * (firstBarId - this.waveform.bars.firstId) / (numBars - width * this.waveform.zoom);
        handle.style.left = left + "px";

        var playPos = document.getElementById("scrollbar-play-position")!;
        playPos.style.visibility = this.waveform.playPos > 0 ? "visible" : "hidden";
        var playPosIdx = this.waveform.playPos - this.waveform.bars.firstId;
        var playPosPercent = 100 * playPosIdx / this.waveform.bars.count();
        playPos.style.left = playPosPercent + "%";

        var selRange = document.getElementById("scrollbar-selected-range")!;
        var rangeValid = this.waveform.selectedRange.playbackRange.min() > 0 && this.waveform.selectedRange.playbackRange.max() > 0;
        selRange.style.visibility = rangeValid ? "visible" : "hidden";
        var leftPosIdx = this.waveform.selectedRange.playbackRange.min() - this.waveform.bars.firstId;
        var rightPosIdx = this.waveform.selectedRange.playbackRange.max() - this.waveform.bars.firstId;
        var leftPosPercent = 100 * leftPosIdx / this.waveform.bars.count();
        var rightPosPercent = 100 * rightPosIdx / this.waveform.bars.count();
        selRange.style.left = leftPosPercent + "%";
        selRange.style.width = (rightPosPercent - leftPosPercent) + "%";
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