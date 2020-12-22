package com.nonlinearlabs.client.world.maps.parameters;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.world.maps.MapsLayout;

public class KeyTrackParameter extends NumericalControlSmall {
	private boolean isVisible = false;

	public KeyTrackParameter(MapsLayout parent, int parameterID) {
		super(parent, parameterID);
	}

	@Override
	public void doFirstLayoutPass(double levelOfDetail) {
		super.doFirstLayoutPass(levelOfDetail);
		isVisible = levelOfDetail >= getLevelOfDetailForFullVisibility();
	}

	@Override
	public void draw(Context2d ctx, Context2d overlay, Context2d menus, int invalidationMask) {
		if (isVisible)
			super.draw(ctx, overlay, menus, invalidationMask);
	}

	@Override
	public double getLevelOfDetailForFullVisibility() {
		return 4;
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
	protected double getInsetY() {
		return 0;
	}
}