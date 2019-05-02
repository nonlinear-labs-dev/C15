package com.nonlinearlabs.NonMaps.client.world.overlay.belt.parameters;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.NonMaps.client.world.overlay.Label;

public class CurrentValuesName extends Label {

	public CurrentValuesName(BeltParameterLayout parent) {
		super(parent);
		super.setFontHeightInMM(5);
	}
	
	@Override
	public BeltParameterLayout getParent() {
		return (BeltParameterLayout) super.getParent();
	}

	@Override
	public String getDrawText(Context2d ctx) {
		return getParent().getCurrentValuesName(getPixRect().getWidth());
	}
}
