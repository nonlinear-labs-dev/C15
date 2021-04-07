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
