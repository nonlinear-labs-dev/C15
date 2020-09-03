package com.nonlinearlabs.client.world.overlay.undo;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.world.Rect;

public class HorizontalScrollbar extends Scrollbar {

	public HorizontalScrollbar(UndoTree undoTree) {
		super(undoTree);
	}

	@Override
	public void draw(Context2d ctx, Context2d overlay, int invalidationMask) {
		super.draw(ctx, overlay, invalidationMask);

		double width = getParent().getScrollPaneWidth();
		double visibleWidth = getParent().getPaneRect().getWidth();
		double offset = getParent().getScrollOffset().getWidth();

		double rectWidth = Math.min(1, visibleWidth / width) * getPixRect().getWidth();
		double x = (getPixRect().getWidth() - rectWidth) * (-offset / (width - visibleWidth));
		Rect r = getPixRect().copy();
		r.setLeft(x + r.getLeft());
		r.setWidth(rectWidth);
		r.fill(ctx, getScrollbarColor());
	}
}
