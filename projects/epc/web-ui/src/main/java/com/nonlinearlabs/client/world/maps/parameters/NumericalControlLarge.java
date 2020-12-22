package com.nonlinearlabs.client.world.maps.parameters;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.maps.MapsLayout;

public class NumericalControlLarge extends ValueDisplayLarge {

	public NumericalControlLarge(MapsLayout parent, int parameterID) {
		super(parent, parameterID);
	}

	@Override
	public void draw(Context2d ctx, Context2d overlay, Context2d menus, int invalidationMask) {
		Rect pixRect = getPixRect();

		ctx.setFillStyle(getColorSliderBackground().toString());
		ctx.setStrokeStyle(getColorObjectContour().toString());

		double insetX = toXPixels(getInsetX());
		double insetY = toYPixels(getInsetY());

		double x = getPixRect().getLeft() + insetX;
		double y = getPixRect().getTop() + insetY;

		ctx.setLineWidth(toXPixels(1));

		ctx.fillRect(x, y, pixRect.getWidth() - 2 * insetX, pixRect.getHeight() - 2 * insetY);
		ctx.strokeRect(x, y, pixRect.getWidth() - 2 * insetX, pixRect.getHeight() - 2 * insetY);

		super.draw(ctx, overlay, menus, invalidationMask);
	}

	@Override
	protected boolean shouldShowTextForLevelOfDetail(double levelOfDetail) {
		return true;
	}

	protected double getInsetX() {
		return 5;
	}

	protected double getInsetY() {
		return 4;
	}

	@Override
	protected double getFontHeight() {
		return 23;
	}

	@Override
	protected double getBasicWidth() {
		return 70;
	}

	@Override
	protected double getBasicHeight() {
		return 35;
	}

	@Override
	protected double getMinHeight() {
		return getBasicHeight();
	}
}
