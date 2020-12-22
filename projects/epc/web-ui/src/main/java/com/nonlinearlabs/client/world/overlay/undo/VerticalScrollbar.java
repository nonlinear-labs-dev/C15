package com.nonlinearlabs.client.world.overlay.undo;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.world.Rect;

public class VerticalScrollbar extends Scrollbar {

	public VerticalScrollbar(UndoTree undoTree) {
		super(undoTree);
	}

	@Override
	public void draw(Context2d ctx, Context2d overlay, Context2d menus, int invalidationMask) {
		super.draw(ctx, overlay, menus, invalidationMask);

		double height = getParent().getScrollPaneHeight();
		double visibleHeight = getParent().getPaneRect().getHeight();
		double offset = getParent().getScrollOffset().getHeight();

		double rectHeight = Math.min(1, visibleHeight / height) * getPixRect().getHeight();
		double y = (getPixRect().getHeight() - rectHeight) * (-offset / (height - visibleHeight));
		Rect r = getPixRect().copy();
		r.setTop(y + r.getTop());
		r.setHeight(rectHeight);
		r.fill(ctx, getScrollbarColor());
	}

}
