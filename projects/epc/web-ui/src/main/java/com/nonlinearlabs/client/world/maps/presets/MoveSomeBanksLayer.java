package com.nonlinearlabs.client.world.maps.presets;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.Millimeter;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.RGBA;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.maps.MapsControl;

public class MoveSomeBanksLayer extends MapsControl {

	private Position startPosition = null;
	private Rect selectionRect = null;

	public MoveSomeBanksLayer(MapsControl parent, Position pos) {
		super(parent);
		startPosition = pos;
		selectionRect = new Rect();
	}

	@Override
	public void draw(Context2d ctx, Context2d overlay, Context2d menus, int invalidationMask) {
		super.draw(ctx, overlay, menus, invalidationMask);
		selectionRect.fillAndStroke(ctx, new RGBA(255, 255, 255, 0.25), Millimeter.toPixels(0.1),
				new RGB(255, 255, 255));
	}

	public void update(Position pos) {
		selectionRect = new Rect(pos, startPosition);
	}

	public Rect getSelectionRect() {
		return selectionRect;
	}

	@Override
	public void doFirstLayoutPass(double levelOfDetail) {
	}
}
