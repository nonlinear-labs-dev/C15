package com.nonlinearlabs.client.world.maps.parameters.EnvB;

import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.parameters.ModulationSourceSlider;
import com.nonlinearlabs.client.world.maps.parameters.ParameterGroupControls;

class EnvBLevelVelLevelKTTimeKT extends ParameterGroupControls {

	private class LvlVel extends ModulationSourceSlider {

		private LvlVel(MapsLayout parent) {
			super(parent, 33);
		}

		@Override
		protected int getBackgroundRoundings() {
			return Rect.ROUNDING_ALL;
		}
	}

	private class LvlKT extends ModulationSourceSlider {

		private LvlKT(MapsLayout parent) {
			super(parent, 36);
		}

		@Override
		protected int getBackgroundRoundings() {
			return Rect.ROUNDING_ALL;
		}
	}

	private class TimeKT extends ModulationSourceSlider {

		private TimeKT(MapsLayout parent) {
			super(parent, 37);
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

	EnvBLevelVelLevelKTTimeKT(MapsLayout parent) {
		super(parent);
		addChild(new LvlVel(this));
		addChild(new LvlKT(this));
		addChild(new TimeKT(this));
	}
}
