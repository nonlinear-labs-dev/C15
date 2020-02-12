package com.nonlinearlabs.client.world.maps.parameters.EnvB;

import com.nonlinearlabs.client.world.maps.LayoutResizingVertical;
import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.parameters.ParameterGroupControls;
import com.nonlinearlabs.client.world.maps.parameters.Spacer;

class EnvBControls extends ParameterGroupControls {

	private class HorizontalBlock extends ParameterGroupControls {
		private HorizontalBlock(VerticalBlock parent) {
			super(parent);

			addChild(new EnvBCol2(this));
			addChild(new EnvBCol3(this));
			addChild(new EnvBCol4(this));
			addChild(new EnvBCol5(this));
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
			addChild(new EnvBLevelVelLevelKTTimeKT(this));
			addChild(new Spacer(this, 1, 15));
		}
	}

	EnvBControls(MapsLayout parent) {
		super(parent);
		addChild(new EnvBCol1(this));
		addChild(new VerticalBlock(this));
		addChild(new EnvBCol6(this));
		addChild(new EnvBCol7(this));

	}
}
