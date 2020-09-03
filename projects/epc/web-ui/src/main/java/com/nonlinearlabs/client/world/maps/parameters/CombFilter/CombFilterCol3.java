package com.nonlinearlabs.client.world.maps.parameters.CombFilter;

import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.parameters.ModulateableKnob;
import com.nonlinearlabs.client.world.maps.parameters.ModulateableSmallKnobParameter;
import com.nonlinearlabs.client.world.maps.parameters.ModulationSourceSlider;
import com.nonlinearlabs.client.world.maps.parameters.ParameterColumn;

class CombFilterCol3 extends ParameterColumn {

	CombFilterCol3(MapsLayout parent) {
		super(parent);
		addChild(new ModulateableKnob(this, 119));
		addChild(new ModulateableSmallKnobParameter(this, 305));
		addChild(new ModulationSourceSlider(this, 121));

	}
}
