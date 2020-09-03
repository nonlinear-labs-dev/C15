package com.nonlinearlabs.client.world.overlay.undo;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.world.Gray;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.overlay.OverlayControl;

public class Scrollbar extends OverlayControl {

	public Scrollbar(UndoTree parent) {
		super(parent);
	}

	@Override
	public UndoTree getParent() {
		return (UndoTree) super.getParent();
	}

	@Override
	public void draw(Context2d ctx, Context2d overlay, int invalidationMask) {
		getPixRect().fill(ctx, new Gray(20));
	}

	public RGB getScrollbarColor() {
		return new RGB(80, 110, 210);
	}
}
