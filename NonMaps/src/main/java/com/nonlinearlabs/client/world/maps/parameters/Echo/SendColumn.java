package com.nonlinearlabs.client.world.maps.parameters.Echo;

import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.parameters.ModulatableSlider;
import com.nonlinearlabs.client.world.maps.parameters.ParameterColumn;

class SendColumn extends ParameterColumn {

	SendColumn(MapsLayout parent) {
		super(parent);
		addChild(new ModulatableSlider(this, 342));
	}
}
