package com.nonlinearlabs.client.world.maps.parameters.Flanger;

import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.parameters.ModulateableKnobWithSlidersHeight;
import com.nonlinearlabs.client.world.maps.parameters.ParameterColumn;

class FlangerCol2 extends ParameterColumn {

	FlangerCol2(MapsLayout parent) {
		super(parent);
		addChild(new ModulateableKnobWithSlidersHeight(this, 211) {
			@Override
			protected int getBackgroundRoundings() {
				return Rect.ROUNDING_LEFT;
			}
		});
	}
}
