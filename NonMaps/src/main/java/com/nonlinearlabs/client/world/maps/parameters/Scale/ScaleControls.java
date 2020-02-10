package com.nonlinearlabs.client.world.maps.parameters.Scale;

import java.util.ArrayList;
import java.util.List;

import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.parameters.ParameterGroupControls;

class ScaleControls extends ParameterGroupControls {

	ScaleControls(MapsLayout parent) {
		super(parent);
		addChild(new ScaleCol0(this));
		addChild(new ScaleCol1(this));
		addChild(new ScaleCol2(this));
		addChild(new ScaleCol3(this));
		addChild(new ScaleCol4(this));
	}

	public List<ScaleParameter> getParameters() {
		List<ScaleParameter> ret = new ArrayList<>();
		for (Control control : getChildren()) {
			if (control instanceof ScaleCol) {
				ret.addAll(((ScaleCol) control).getScaleParams());
			}
		}
		return ret;
	}
}
