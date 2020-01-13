package com.nonlinearlabs.client.world.maps.parameters.FBMixer;

import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.parameters.ModulatableSlider;
import com.nonlinearlabs.client.world.maps.parameters.ModulationSourceSlider;
import com.nonlinearlabs.client.world.maps.parameters.ParameterColumn;

class FBMixerColumn7 extends ParameterColumn {

	FBMixerColumn7(MapsLayout parent) {
		super(parent);
		addChild(new ModulatableSlider(this, 299));
		addChild(new ModulationSourceSlider(this, 168));
	}
}
