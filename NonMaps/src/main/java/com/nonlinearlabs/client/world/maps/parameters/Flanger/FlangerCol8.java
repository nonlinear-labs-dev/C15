package com.nonlinearlabs.client.world.maps.parameters.Flanger;

import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.parameters.ModulateableKnob;
import com.nonlinearlabs.client.world.maps.parameters.ParameterColumn;

class FlangerCol8 extends ParameterColumn {

    private class Tremolo extends ModulateableKnob {

        private Tremolo(MapsLayout parent) {
            super(parent, 389);
        }
    }

	FlangerCol8(MapsLayout parent) {
		super(parent);
		addChild(new Tremolo(this));
	}
}
