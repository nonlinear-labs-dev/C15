package com.nonlinearlabs.client.world.maps.parameters;

import com.nonlinearlabs.client.world.maps.MapsLayout;

public class BooleanControlSmall extends NumericalControlSmall {

	public BooleanControlSmall(MapsLayout parent, int parameterID) {
		super(parent, parameterID);
	}

    @Override
    public String getDisplayValue() {
        return Double.parseDouble(super.getDisplayValue()) == 0 ? "off" : "on";
    }

	@Override
	protected double getFontHeight() {
		return 11;
	}

	@Override
	protected double getInsetX() {
		return 5;
	}

	@Override
	protected double getInsetY() {
		return 6;
	}

	@Override
	protected double getBasicWidth() {
		return 80;
	}

	@Override
	protected double getBasicHeight() {
		return 27;
	}
}
