package com.nonlinearlabs.client.world.maps.parameters.OscA;

import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.parameters.ModulateableKnob;
import com.nonlinearlabs.client.world.maps.parameters.ModulationSourceSlider;
import com.nonlinearlabs.client.world.maps.parameters.ModulationSourceSwitch;
import com.nonlinearlabs.client.world.maps.parameters.ParameterColumn;

class FluctColumn extends ParameterColumn {

	FluctColumn(MapsLayout parent) {
		super(parent);
		addChild(new ModulateableKnob(this, 57));
		addChild(new ModulationSourceSlider(this, 59));
		addChild(new ModulateableKnob(this, 301));
		addChild(new ModulationSourceSwitch(this, 393));
	}
}
