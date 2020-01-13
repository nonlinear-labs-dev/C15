package com.nonlinearlabs.client.world.maps.parameters.FBMixer;

import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.parameters.DoubleParameterColumn;
import com.nonlinearlabs.client.world.maps.parameters.ModulateableKnob;
import com.nonlinearlabs.client.world.maps.parameters.ParameterColumn;
import com.nonlinearlabs.client.world.maps.parameters.SizeLinkedParameterColumn;
import com.nonlinearlabs.client.world.maps.parameters.SmallKnobParameter;

class FBMixerColumn6 extends DoubleParameterColumn {

	class DriveParameterColumn extends SizeLinkedParameterColumn {

		private class Drive extends ModulateableKnob {

			private Drive(MapsLayout parent) {
				super(parent, 164);
			}

			@Override
			protected int getBackgroundRoundings() {
				return Rect.ROUNDING_LEFT;
			}
		}

		public DriveParameterColumn(MapsLayout parent) {
			super(parent);
			addChild(new Drive(this));
		}
	}

	class FoldAndAsymetryColumn extends ParameterColumn {

		private class Fold extends SmallKnobParameter {

			private Fold(MapsLayout parent) {
				super(parent, 166);
			}

			@Override
			protected int getBackgroundRoundings() {
				return Rect.ROUNDING_RIGHT_TOP;
			}
		}

		private class Asym extends SmallKnobParameter {

			private Asym(MapsLayout parent) {
				super(parent, 167);
			}

			@Override
			protected int getBackgroundRoundings() {
				return Rect.ROUNDING_RIGHT_BOTTOM;
			}
		}

		public FoldAndAsymetryColumn(MapsLayout parent) {
			super(parent);
			addChild(new Fold(this));
			addChild(new Asym(this));
		}
	}

	FBMixerColumn6(MapsLayout parent) {
		super(parent);
		addChild(new DriveParameterColumn(this));
		addChild(new FoldAndAsymetryColumn(this));
	}
}
