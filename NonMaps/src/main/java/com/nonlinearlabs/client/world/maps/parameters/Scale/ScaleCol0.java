package com.nonlinearlabs.client.world.maps.parameters.Scale;

import com.nonlinearlabs.client.world.maps.MapsLayout;

class ScaleCol0 extends ScaleCol {

	ScaleCol0(MapsLayout parent) {
		super(parent);

		addChild(new ScaleParameter(this, 312));
	}
}
