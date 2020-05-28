package com.nonlinearlabs.client.world.maps.parameters.OscB;

import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.parameters.ModulateableKnob;
import com.nonlinearlabs.client.world.maps.parameters.ModulationSourceSlider;
import com.nonlinearlabs.client.world.maps.parameters.ModulationSourceSwitch;
import com.nonlinearlabs.client.world.maps.parameters.ParameterColumn;

class FluctColumn extends ParameterColumn {

	FluctColumn(MapsLayout parent) {
		super(parent);
		addChild(new ModulateableKnob(this, 87));
		addChild(new ModulationSourceSlider(this, 89));
		addChild(new ModulateableKnob(this, 302));
		addChild(new ModulationSourceSwitch(this, 394));
	}
}
