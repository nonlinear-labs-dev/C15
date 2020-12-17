package com.nonlinearlabs.client.world.maps.parameters.Echo;

import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.parameters.ParameterColumn;
import com.nonlinearlabs.client.world.maps.parameters.SmallKnobParameter;

class StereoColumn extends ParameterColumn {

	StereoColumn(MapsLayout parent) {
		super(parent);
		addChild(new SmallKnobParameter(this, 231) {
			@Override
			protected int getBackgroundRoundings() {
				return Rect.ROUNDING_RIGHT_TOP;
			}
		});
		addChild(new SmallKnobParameter(this, 232) {
			@Override
			protected int getBackgroundRoundings() {
				return Rect.ROUNDING_RIGHT_BOTTOM;
			}
		});
	}
}
