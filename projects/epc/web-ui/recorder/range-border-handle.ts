
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

    setTime(time: string) {
        this.time = time;
    }

    private time = "";
}
