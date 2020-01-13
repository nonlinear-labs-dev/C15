package com.nonlinearlabs.client.world.maps.parameters.MonoAndUnison;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.world.Gray;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.parameters.LabelSmall;


public class UnisonAndMonoModuleHeader extends LabelSmall {

	public UnisonAndMonoModuleHeader(MapsLayout parent) {
		super(parent);
	}

    @Override
    public String getText() {
        return "Voices";
	}
	
	@Override
	protected String getDisplayText() {
		return getText();
	}

	@Override
	public void draw(Context2d ctx, int invalidationMask) {
		Rect pixRect = getPixRect();
		pixRect.drawRoundedRect(ctx, Rect.ROUNDING_TOP, toXPixels(6), toXPixels(2), new Gray(87), null);
		super.draw(ctx, invalidationMask);
	}

	@Override
	protected double getBasicWidth() {
		return 70;
	}

	@Override
	protected double getBasicHeight() {
		return 28;
	}

	@Override
	protected boolean shouldShowTextForLevelOfDetail(double levelOfDetail) {
		return true;
	}

	@Override
	public double getLevelOfDetailForFullVisibility() {
		return 1;
	}

	@Override
	public void doSecondLayoutPass(double parentsWidthFromFirstPass, double parentsHeightFromFirstPass) {
		setNonSize(parentsWidthFromFirstPass, getNonPosition().getDimension().getHeight());
	}

	protected String getLockSymbol() {
		return "\ue20A";
	}

	protected float getLockSymbolOffsetFactor() {
		return (float) 1;
	}

	@Override
	protected double getFontHeight() {
		return 16;
	}
}
