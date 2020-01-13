package com.nonlinearlabs.client.world.maps.parameters.EnvA;

import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.parameters.ModulateableKnob;
import com.nonlinearlabs.client.world.maps.parameters.ModulationSourceSlider;
import com.nonlinearlabs.client.world.maps.parameters.ParameterColumn;

class EnvACol2 extends ParameterColumn {

	EnvACol2(MapsLayout parent) {
		super(parent);
		addChild(new ModulateableKnob(this, 2));
		addChild(new ModulationSourceSlider(this, 328));
	}
}
