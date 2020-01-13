package com.nonlinearlabs.client.world.maps.parameters.FBMixer;

import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.parameters.ModulatableSlider;
import com.nonlinearlabs.client.world.maps.parameters.ParameterColumn;
import com.nonlinearlabs.client.world.maps.parameters.SmallHorizontalDotSlider;

class FBMixerColumn3 extends ParameterColumn {

	FBMixerColumn3(MapsLayout parent) {
		super(parent);
		addChild(new ModulatableSlider(this, 158));
		addChild(new SmallHorizontalDotSlider(this, 352));
	}
}
