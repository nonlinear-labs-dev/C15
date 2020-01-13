package com.nonlinearlabs.client.world.maps.parameters.EnvB;

import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.parameters.ModulateableKnob;
import com.nonlinearlabs.client.world.maps.parameters.ParameterColumn;
import com.nonlinearlabs.client.world.maps.parameters.SmallKnobParameter;

class EnvBCol5 extends ParameterColumn {

	EnvBCol5(MapsLayout parent) {
		super(parent);
		addChild(new ModulateableKnob(this, 27));
		addChild(new SmallKnobParameter(this, 338));
	}
}
