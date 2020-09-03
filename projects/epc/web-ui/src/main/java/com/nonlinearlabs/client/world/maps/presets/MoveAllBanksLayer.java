package com.nonlinearlabs.client.world.maps.presets;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.Millimeter;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Dimension;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.RGBA;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.maps.MapsControl;
import com.nonlinearlabs.client.world.maps.NonDimension;

public class MoveAllBanksLayer extends MapsControl {

	private Position start = null;

	public MoveAllBanksLayer(MapsControl parent) {
		super(parent);
	}

	@Override
	public void doFirstLayoutPass(double levelOfDetail) {
	}

	@Override
	public void draw(Context2d ctx, Context2d overlay, int invalidationMask) {
		super.draw(ctx, overlay, invalidationMask);

		double twoMM = Millimeter.toPixels(2);
		Rect minRect = getPixRect().copy();
		minRect.applyPadding(-twoMM, -twoMM, -twoMM, -twoMM);
		minRect.drawRoundedArea(ctx, twoMM, Millimeter.toPixels(0.1), new RGBA(255, 255, 255, 0.25),
				new RGB(255, 255, 255));
	}

	@Override
	public Control mouseDown(Position eventPoint) {
		start = eventPoint;
		return this;
	}

	@Override
	public Control mouseUp(Position eventPoint) {
		double x = eventPoint.getX() - start.getX();
		double y = eventPoint.getY() - start.getY();
		Dimension d = new Dimension(x, y);
		NonDimension distance = toNonDimension(d);
		NonMaps.get().getServerProxy().moveAllBanks(distance);
		return this;
	}

	@Override
	public Control mouseDrag(Position oldPoint, Position newPoint, boolean fine) {
		double x = newPoint.getX() - oldPoint.getX();
		double y = newPoint.getY() - oldPoint.getY();
		Dimension d = new Dimension(x, y);
		NonDimension distance = toNonDimension(d);
		NonMaps.get().getNonLinearWorld().getPresetManager().moveAllBanksBy(distance);
		return this;
	}

	@Override
	public Control startDragging(Position pos) {
		return this;
	}
}
