package com.nonlinearlabs.client.world.maps.parameters;

import com.google.gwt.canvas.dom.client.Context2d;
import com.google.gwt.canvas.dom.client.Context2d.TextAlign;
import com.google.gwt.canvas.dom.client.Context2d.TextBaseline;
import com.nonlinearlabs.client.Checksum;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.maps.MapsLayout;

class ValueDisplay extends ValueControl {

	private boolean isVisible = false;

	ValueDisplay(MapsLayout parent, int parameterID) {
		super(parent, parameterID);
	}

	@Override
	public void doFirstLayoutPass(double levelOfDetail) {
		super.doFirstLayoutPass(levelOfDetail);
		isVisible = shouldShowTextForLevelOfDetail(levelOfDetail);
	}

	private RGB getFontColor() {
		if (getParentParameterLayout().isSelected())
			return getColorFontValueDisplaySelected();
		return getColorFontValueDisplay();
	}

	protected boolean shouldShowTextForLevelOfDetail(double levelOfDetail) {
		return levelOfDetail >= getLevelOfDetailForFullVisibility();
	}

	protected double getFontHeight() {
		return 8;
	}

	@Override
	protected double getBasicWidth() {
		return 60;
	}

	@Override
	protected double getBasicHeight() {
		return 15;
	}

	@Override
	protected double getMinHeight() {
		return 0;
	}

	@Override
	public void getStateHash(Checksum crc) {
		super.getStateHash(crc);
		crc.eat(getValue());
	}

	@Override
	public boolean isVisible() {
		return isVisible && super.isVisible();
	}

	@Override
	public void draw(Context2d ctx, int invalidationMask) {

		if (isVisible) {
			Rect pixRect = getPixRect();

			double fontHeightInUnits = getFontHeight();
			double fontHeightInPixels = toYPixels(fontHeightInUnits);

			ctx.setTextAlign(TextAlign.CENTER);
			ctx.setFillStyle(getFontColor().toString());
			ctx.setTextBaseline(TextBaseline.MIDDLE);

			ctx.setFont(fontHeightInPixels + "px 'SSP-LW25'");
			ctx.fillText(getDisplayValue(), pixRect.getCenterPoint().getX(),
					pixRect.getCenterPoint().getY() + toYPixels(moveFontVerticallyBy()));
		}
	}

	protected double moveFontVerticallyBy() {
		return 0;
	}
}
