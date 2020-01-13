package com.nonlinearlabs.client.world.maps.parameters.EnvA;

import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.parameters.ModulateableKnob;
import com.nonlinearlabs.client.world.maps.parameters.ParameterColumn;
import com.nonlinearlabs.client.world.maps.parameters.SmallKnobParameter;

class EnvACol5 extends ParameterColumn {

	EnvACol5(MapsLayout parent) {
		super(parent);
		addChild(new ModulateableKnob(this, 8));
		addChild(new SmallKnobParameter(this, 332));
	}
}
