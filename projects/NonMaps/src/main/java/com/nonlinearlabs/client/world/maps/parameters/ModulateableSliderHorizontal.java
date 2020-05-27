package com.nonlinearlabs.client.world.maps.parameters;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.world.Dimension;
import com.nonlinearlabs.client.world.Gray;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.maps.MapsLayout;

public class ModulateableSliderHorizontal extends SliderHorizontal {

	private boolean modVisible = false;

	public ModulateableSliderHorizontal(MapsLayout parent, int parameterID) {
		super(parent, parameterID);
	}

	@Override
	public void draw(Context2d ctx, int invalidationMask) {
		super.draw(ctx, invalidationMask);

		if (presenter.modulation.isModulated && modVisible) {
			drawModIndication(ctx);
		}
	}

	private void drawModIndication(Context2d ctx) {
		boolean isBiPolar = presenter.bipolar;
		double targetValue = presenter.controlPosition;
		double modLeft = presenter.modulation.modulationRange.left;
		double modRight = presenter.modulation.modulationRange.right;

		Rect r = getPixRect().copy();

		double sliderLeftStart = isBiPolar() ? getPixRect().getCenterPoint().getX()
				: getPixRect().getLeft() + toXPixels(19);
		double sliderRightEnd = getPixRect().getRight() - toXPixels(19);

		double width = sliderRightEnd - sliderLeftStart;
		double targetX = 0;

		if (isBiPolar) {
			double x = r.getCenterPoint().getX();
			r.setLeft(x + width / 2 * modLeft);
			r.setRight(x + width / 2 * modRight);
			targetX = x + width / 2 * targetValue;
		} else {
			r.setLeft(sliderLeftStart + width * modLeft);
			r.setRight(sliderLeftStart + width * modRight);
			targetX = sliderLeftStart + width * targetValue;
		}

		r = normalizeRect(r);

		r.moveBy(0, toYPixels(-2.35));
		r.setHeight(r.getHeight() * 0.3);

		RGB fillColor = getColorCorona();

		Rect leftRect = r.copy();
		leftRect.setRight(targetX);
		leftRect.fill(ctx, fillColor);

		Rect rightRect = r.copy();
		double oldWidth = rightRect.getRight() - targetX;

		rightRect.setLeft(targetX);
		rightRect.setWidth(oldWidth);
		rightRect.fill(ctx, fillColor);

		leftRect.moveBy(0, toYPixels(8.9));
		rightRect.moveBy(0, toYPixels(8.9));

		leftRect.fill(ctx, fillColor);
		rightRect.fill(ctx, fillColor);
	}

	protected Rect normalizeRect(Rect r) {
		r.normalize();
		r = r.getIntersection(getPixRect());
		return r;
	}

	protected double getVerticalOffset() {
		return 0;
	}

	protected RGB getColorCorona() {
		return new Gray(200);
	}

	protected Rect getSliderRect() {
		Rect r = new Rect(getPixRect().getPosition(), getSliderDimension());
		r.centerIn(getPixRect());
		r.moveBy(0, toYPixels(getVerticalOffset()));
		return r;
	}

	private Dimension getSliderDimension() {
		return new Dimension(toXPixels(getBasicWidth() - 4), toYPixels(getBasicHeight()));
	}

	protected double getSliderHandleWidth() {
		return 7;
	}

	@Override
	public void doFirstLayoutPass(double levelOfDetail) {
		super.doFirstLayoutPass(levelOfDetail);
		modVisible = levelOfDetail >= 5;
	}
}