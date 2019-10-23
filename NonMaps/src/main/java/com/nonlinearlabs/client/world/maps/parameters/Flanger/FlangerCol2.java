package com.nonlinearlabs.client.world.maps.parameters.Flanger;

import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.parameters.ModulateableKnob;
import com.nonlinearlabs.client.world.maps.parameters.ParameterColumn;

class FlangerCol2 extends ParameterColumn {

	FlangerCol2(MapsLayout parent) {
		super(parent);
		addChild(new ModulateableKnob(this, 211));
	}
}
