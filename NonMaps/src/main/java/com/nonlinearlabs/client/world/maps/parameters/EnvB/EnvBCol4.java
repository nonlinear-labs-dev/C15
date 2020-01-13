package com.nonlinearlabs.client.world.maps.parameters.EnvB;

import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.parameters.ModulateableKnob;
import com.nonlinearlabs.client.world.maps.parameters.ModulationSourceSlider;
import com.nonlinearlabs.client.world.maps.parameters.ParameterColumn;

class EnvBCol4 extends ParameterColumn {

	EnvBCol4(MapsLayout parent) {
		super(parent);
		addChild(new ModulateableKnob(this, 25));
		addChild(new ModulationSourceSlider(this, 336));
	}
}
