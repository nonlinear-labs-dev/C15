package com.nonlinearlabs.client.world.maps.parameters.ShapeA;

import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.parameters.ModulateableKnobWithSlidersHeight;
import com.nonlinearlabs.client.world.maps.parameters.ParameterColumn;

class RingModColumn extends ParameterColumn {

	RingModColumn(MapsLayout parent) {
		super(parent);
		addChild(new ModulateableKnobWithSlidersHeight(this, 81));
	}
}
 