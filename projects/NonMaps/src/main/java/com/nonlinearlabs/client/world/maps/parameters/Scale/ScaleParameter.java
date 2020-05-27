package com.nonlinearlabs.client.world.maps.parameters.Scale;

import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.parameters.LabelModulationSource;
import com.nonlinearlabs.client.world.maps.parameters.ModulationSourceHighPriority;
import com.nonlinearlabs.client.world.maps.parameters.SliderHorizontal;
import com.nonlinearlabs.client.world.maps.parameters.ValueDisplaySmall;

public class ScaleParameter extends ModulationSourceHighPriority {

	public ScaleParameter(MapsLayout parent, int parameterID) {
		super(parent, parameterID);
		addChild(new LabelModulationSource(this, getParameterNumber()));
		addChild(new SliderHorizontal(this, getParameterNumber()));
		addChild(new ValueDisplaySmall(this, getParameterNumber()));
	}

	public double getDefaultValue() {
		return 0.0;
	}

}