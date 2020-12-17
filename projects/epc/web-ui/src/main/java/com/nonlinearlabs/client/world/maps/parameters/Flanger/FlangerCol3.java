package com.nonlinearlabs.client.world.maps.parameters.Flanger;

import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.parameters.ModulateableSmallKnobParameter;
import com.nonlinearlabs.client.world.maps.parameters.ParameterColumn;
import com.nonlinearlabs.client.world.maps.parameters.SmallKnobParameter;

class FlangerCol3 extends ParameterColumn {

	FlangerCol3(MapsLayout parent) {
		super(parent);
		addChild(new ModulateableSmallKnobParameter(this, 216) {
			@Override
			protected int getBackgroundRoundings() {
				return Rect.ROUNDING_RIGHT_TOP;
			}
		});
		addChild(new SmallKnobParameter(this, 218) {
			@Override
			protected int getBackgroundRoundings() {
				return Rect.ROUNDING_RIGHT_BOTTOM;
			}
		});
	}
}
