package com.nonlinearlabs.client.world.maps.parameters.Master;

import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.parameters.ModulatableSlider;
import com.nonlinearlabs.client.world.maps.parameters.ParameterColumn;
import com.nonlinearlabs.client.world.maps.parameters.SmallModulateableParameter;

class MasterCol1 extends ParameterColumn {

	MasterCol1(MapsLayout parent) {
		super(parent);
		addChild(new ModulatableSlider(this, 247));
		addChild(new SmallModulateableParameter(this, 248));
	}
}
