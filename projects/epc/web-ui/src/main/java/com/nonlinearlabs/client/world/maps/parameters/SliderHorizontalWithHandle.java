package com.nonlinearlabs.client.world.maps.parameters;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.presenters.ParameterPresenter;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.maps.MapsLayout;

public class SliderHorizontalWithHandle extends SliderHorizontal {
	public SliderHorizontalWithHandle(MapsLayout parent, int parameterID) {
		super(parent, parameterID);
	}

	@Override
	protected double getBasicWidth() {
		return 120;
	}

	@Override
	protected double getBasicHeight() {
		return 9;
	}

	@Override
	protected void drawIndicatorArea(Context2d ctx, double centerX, double indicatorAreaWidth, double indicatorStartX) {
	}

	@Override
	protected void drawHandle(Context2d ctx, double indicatorAreaWidth, double indicatorStartX) {
		Rect pixRect = getPixRect();

		Parameter p = getParentParameterLayout();

		if (Math.abs(getValue()) < 0.001 && p.dimHandleAtDefaultValue()) {
			ctx.setStrokeStyle(getColorObjectContour().toString());
			ctx.setFillStyle(getColorObjectContour().toString());
		} else {
			ctx.setStrokeStyle(getColorObjectContour().toString());
			ctx.setFillStyle(getColorIndicator().toString());
		}

		double x = indicatorStartX + toXPixels(indicatorAreaWidth);
		ctx.beginPath();
		ctx.arc(x, pixRect.getCenterPoint().getY(), pixRect.getHeight() / 2, 0, 2 * Math.PI);
		ctx.fill();
		ctx.stroke();
	}

	@Override
	protected void drawBackground(Context2d ctx) {
		Rect background = calcBackgroundRect();
		double rounding = background.getHeight() / 2;
		background.applyPadding(-rounding, 0, -rounding, 0);
		background.drawRoundedArea(ctx, rounding, toXPixels(1), getColorSliderBackground(), getColorObjectContour());
	}

	@Override
	public void draw(Context2d ctx, Context2d overlay, Context2d menus, int invalidationMask) {
		super.draw(ctx, overlay, menus, invalidationMask);

		ParameterPresenter p = getParentParameterLayout().presenter;

		Rect bgRect = calcBackgroundRect();
		Rect pixRect = getPixRect().copy();
		pixRect.setLeft(bgRect.getLeft() + toXPixels(1));
		pixRect.setWidth(bgRect.getWidth() - toXPixels(2));

		double top = pixRect.getTop();
		double bottom = pixRect.getBottom();

		if (p.drawCenterReturnIndicator)
			drawReturnArrow(ctx, pixRect.getCenterPoint().getX(), top, bottom);

		if (p.drawZeroReturnIndicator)
			drawReturnArrow(ctx, pixRect.getLeft(), top, bottom);
	}

	protected void drawReturnArrow(Context2d ctx, double x, double top, double bottom) {
		double strength = toXPixels(4);
		double halfStrength = strength / 2;

		ctx.setLineWidth(toXPixels(0.5));
		ctx.setFillStyle(getColorIndicator().toString());
		ctx.setStrokeStyle(getColorObjectContour().toString());

		ctx.beginPath();
		ctx.moveTo(x - strength, top - strength);
		ctx.lineTo(x + strength, top - strength);
		ctx.lineTo(x, top + halfStrength);
		ctx.closePath();

		ctx.fill();
		ctx.stroke();

		ctx.beginPath();
		ctx.moveTo(x - strength, bottom + strength);
		ctx.lineTo(x + strength, bottom + strength);
		ctx.lineTo(x, bottom - halfStrength);
		ctx.closePath();

		ctx.fill();
		ctx.stroke();
	}
}