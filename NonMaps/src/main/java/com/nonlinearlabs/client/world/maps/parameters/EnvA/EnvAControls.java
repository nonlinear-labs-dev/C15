package com.nonlinearlabs.client.world.maps.parameters.EnvA;

import com.nonlinearlabs.client.world.maps.LayoutResizingVertical;
import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.parameters.ParameterGroupControls;
import com.nonlinearlabs.client.world.maps.parameters.Spacer;

class EnvAControls extends ParameterGroupControls {

	private class HorizontalBlock extends ParameterGroupControls {
		private HorizontalBlock(VerticalBlock parent) {
			super(parent);

			addChild(new EnvACol2(this));
			addChild(new EnvACol3(this));
			addChild(new EnvACol4(this));
			addChild(new EnvACol5(this));
		}

		@Override
		public double getXMargin() {
			return 0;
		}
	}

	private class VerticalBlock extends LayoutResizingVertical {
		private VerticalBlock(MapsLayout parent) {
			super(parent);

			addChild(new HorizontalBlock(this));
			addChild(new Spacer(this, 1, 45 / 2.0));
			addChild(new EnvALevelVelLevelKTTimeKT(this));
			addChild(new Spacer(this, 1, 45 / 3.0));
		}
	}

	EnvAControls(MapsLayout parent) {
		super(parent);
		addChild(new EnvACol1(this));

		addChild(new VerticalBlock(this));

		addChild(new EnvACol6(this));
		addChild(new EnvACol7(this));

	}
}
