package com.nonlinearlabs.client.world.maps.parameters.OutputMixer;

import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.parameters.ModulatableSlider;
import com.nonlinearlabs.client.world.maps.parameters.ModulationSourceSlider;
import com.nonlinearlabs.client.world.maps.parameters.ParameterColumn;

class LevelColumn extends ParameterColumn {

	LevelColumn(MapsLayout parent) {
		super(parent);
		addChild(new ModulatableSlider(this, 185));
		addChild(new ModulationSourceSlider(this, 187));
		addChild(new ModulationSourceSlider(this, 362));
	}
}
