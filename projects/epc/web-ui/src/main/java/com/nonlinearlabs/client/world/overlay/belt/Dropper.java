package com.nonlinearlabs.client.world.overlay.belt;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.overlay.DragProxy;
import com.nonlinearlabs.client.world.overlay.SVGImage;

abstract class Dropper extends SVGImage {

	Dropper(Control parent, String imageNormal, String imageTouch) {
		super(parent, imageNormal, imageTouch);
	}

	@Override
	public void draw(Context2d ctx, Context2d overlay, Context2d menus, int invalidationMask) {
		super.draw(ctx, overlay, menus, invalidationMask);
	}

	@Override
	public Control drag(Position pos, DragProxy dragProxy) {
		return this;
	}

	protected void setIsDropTarget(boolean b) {
	}

	@Override
	public void dragLeave() {
		super.dragLeave();
		setIsDropTarget(false);
		invalidate(INVALIDATION_FLAG_UI_CHANGED);
	}

	@Override
	public Control drop(Position pos, DragProxy dragProxy) {
		if (doesDragProxyMatch(dragProxy)) {
			doAction(dragProxy);
			setIsDropTarget(false);
			invalidate(INVALIDATION_FLAG_UI_CHANGED);
			return this;
		}
		return this;
	}

	protected abstract boolean doesDragProxyMatch(DragProxy dragProxy);

	protected abstract void doAction(DragProxy dragProxy);

}