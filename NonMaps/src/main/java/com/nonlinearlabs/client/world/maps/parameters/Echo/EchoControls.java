package com.nonlinearlabs.client.world.maps.parameters.Echo;

import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.parameters.ParameterGroupControls;

class EchoControls extends ParameterGroupControls {

	EchoControls(MapsLayout parent) {
		super(parent);
		addChild(new TimeColumn(this));
		addChild(new StereoColumn(this));
		addChild(new FeedbackColumn(this));
		addChild(new SendColumn(this));
		addChild(new MixColumn(this));
	}
}
