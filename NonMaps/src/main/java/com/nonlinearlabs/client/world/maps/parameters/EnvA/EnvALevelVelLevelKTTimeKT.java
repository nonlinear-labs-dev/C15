package com.nonlinearlabs.client.world.maps.parameters.EnvA;

import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.parameters.LabelModulationSource;
import com.nonlinearlabs.client.world.maps.parameters.ModulationSourceHighPriority;
import com.nonlinearlabs.client.world.maps.parameters.ModulationSourceSlider;
import com.nonlinearlabs.client.world.maps.parameters.ParameterGroupControls;
import com.nonlinearlabs.client.world.maps.parameters.SliderHorizontal;
import com.nonlinearlabs.client.world.maps.parameters.ValueDisplaySmall;

class EnvALevelVelLevelKTTimeKT extends ParameterGroupControls {

	private class LvlVel extends ModulationSourceHighPriority {

		private LvlVel(MapsLayout parent) {
			super(parent, 14);
			addChild(new LabelModulationSource(this, getParameterNumber()));
			addChild(new SliderHorizontal(this, getParameterNumber()));
			addChild(new ValueDisplaySmall(this, getParameterNumber()));
		}

		@Override
		protected int getBackgroundRoundings() {
			return Rect.ROUNDING_ALL;
		}
	}

	private class LvlKT extends ModulationSourceSlider {

		private LvlKT(MapsLayout parent) {
			super(parent, 17);
		}

		@Override
		protected int getBackgroundRoundings() {
			return Rect.ROUNDING_ALL;
		}
	}

	private class TimeKT extends ModulationSourceSlider {

		private TimeKT(MapsLayout parent) {
			super(parent, 18);
		}

		@Override
		protected int getBackgroundRoundings() {
			return Rect.ROUNDING_ALL;
		}
	}

	@Override
	public double getLeftMargin() {
		return 82;
	}

	public double getRightMargin() {
		return -1;
	}

	EnvALevelVelLevelKTTimeKT(MapsLayout parent) {
		super(parent);
		addChild(new LvlVel(this));
		addChild(new LvlKT(this));
		addChild(new TimeKT(this));
	}
}
