package com.nonlinearlabs.client.world.maps;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.contextStates.ClipContext;
import com.nonlinearlabs.client.world.maps.parameters.LabelLarge;

public class Label extends LabelLarge {

	public Label(MapsLayout parent, String text) {
		super(parent, text);
	}

	@Override
	public void draw(Context2d ctx, int invalidationMask) {
		try (ClipContext clipper = new ClipContext(ctx, this)) {
			super.draw(ctx, invalidationMask);
		}
	}

	@Override
	public double getLevelOfDetailForFullVisibility() {
		return getParent().getLevelOfDetailForFullVisibility();
	}

	@Override
	protected boolean shouldShowTextForLevelOfDetail(double levelOfDetail) {
		return true;
	}

	@Override
	public void doFirstLayoutPass(double levelOfDetail) {
		super.doFirstLayoutPass(levelOfDetail);
		setNonSize(getNonWidth() + getWidthMargin(), getNonPosition().getHeight());
	}

	protected int getNonWidth() {
		return 180;
	}

	@Override
	protected double getFontHeight() {
		return 16;
	}

	@Override
	protected String getFontName() {
		return "'SSP-LW25'";
	}

	@Override
	protected double getBasicHeight() {
		return 30;
	}

}
