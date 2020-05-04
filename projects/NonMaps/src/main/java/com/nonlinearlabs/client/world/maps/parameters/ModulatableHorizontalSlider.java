package com.nonlinearlabs.client.world.maps.parameters;

import com.nonlinearlabs.client.world.maps.MapsLayout;

public class ModulatableHorizontalSlider extends ModulatableParameter {

	public ModulatableHorizontalSlider(MapsLayout parent, int parameterID) {
		super(parent, parameterID);
		addChild(new ModulationSourceLabel(this, getParameterNumber()));
		addChild(new ModulateableSliderHorizontal(this, getParameterNumber()));
		addChild(new ValueDisplayLarge(this, getParameterNumber()));
	}
}
