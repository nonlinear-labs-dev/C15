package com.nonlinearlabs.client.world.maps.presets.bank;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Gray;
import com.nonlinearlabs.client.world.IPreset;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.RGBA;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.maps.Label;
import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.overlay.DragProxy;
import com.nonlinearlabs.client.world.overlay.belt.EditBufferDraggingButton;

final class EmptyLabel extends Label {
	private final Bank bank;
	private boolean dropping = false;

	EmptyLabel(Bank bank, MapsLayout parent, String text) {
		super(parent, text);
		this.bank = bank;
	}

	@Override
	protected int getNonWidth() {
		return 232;
	}

	@Override
	public void draw(Context2d ctx, Context2d overlay, int invalidationMask) {
		Rect rect = getPixRect();

		if (dropping) {
			rect.fill(ctx, new RGBA(255, 0, 0, 0.75));
		} else {
			rect.fill(ctx, new Gray(25));
		}

		super.draw(ctx, overlay, invalidationMask);
	}

	@Override
	public RGB getColorFont() {
		return new Gray(128);
	}

	@Override
	public Control drop(Position pos, DragProxy dragProxy) {
		if (dragProxy.getOrigin() instanceof IPreset) {
			IPreset p = (IPreset) dragProxy.getOrigin();
			getNonMaps().getServerProxy().appendPreset(p, this.bank);
			return this;
		} else if (dragProxy.getOrigin() instanceof EditBufferDraggingButton) {
			getNonMaps().getServerProxy().appendEditBuffer(this.bank);
			return this;
		}

		return null;
	}

	@Override
	public Control drag(Position pos, DragProxy dragProxy) {
		if (dragProxy.getOrigin() instanceof IPreset || dragProxy.getOrigin() instanceof EditBufferDraggingButton) {
			dropping = true;
			return this;
		}
		return super.drag(pos, dragProxy);
	}

	@Override
	public Control mouseDrag(Position oldPoint, Position newPoint, boolean fine) {
		return super.mouseDrag(oldPoint, newPoint, fine);
	}

	@Override
	public void dragLeave() {
		dropping = false;
		invalidate(INVALIDATION_FLAG_UI_CHANGED);
	}
}