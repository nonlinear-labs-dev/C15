package com.nonlinearlabs.client.world.maps.parameters.FBMixer;

import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.parameters.ParameterGroupControls;

class FBMixerControls extends ParameterGroupControls {

	FBMixerControls(MapsLayout parent) {
		super(parent);
		addChild(new FBMixerColumn1(this));
		addChild(new FBMixerColumn2(this));
		addChild(new FBMixerColumn3(this));
		addChild(new FBMixerColumn4(this));
		addChild(new FBMixerColumn6(this));
		addChild(new FBMixerColumn7(this));
	}
}
