package com.nonlinearlabs.client.world.maps.parameters;

import com.nonlinearlabs.client.world.maps.MapsLayout;

public class ModulationSourceSwitch extends ModulationSourceHighPriority {

	public ModulationSourceSwitch(MapsLayout parent, int parameterID) {
		super(parent, parameterID);
        addChild(new LabelModulationSource(this, getParameterNumber()));
        addChild(new KeyTrackParameter(this, getParameterNumber()));
	}
}
