package com.nonlinearlabs.client.world.maps.parameters.FBMixer;

import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.parameters.ModulatableSlider;
import com.nonlinearlabs.client.world.maps.parameters.ModulationSourceSlider;
import com.nonlinearlabs.client.world.maps.parameters.ParameterColumn;
import com.nonlinearlabs.client.world.maps.parameters.SmallHorizontalDotSlider;

class FBMixerColumn4 extends ParameterColumn {

	FBMixerColumn4(MapsLayout parent) {
		super(parent);
		addChild(new ModulatableSlider(this, 160));
		addChild(new ModulationSourceSlider(this, 162));
		addChild(new SmallHorizontalDotSlider(this, 354));
	}
}
