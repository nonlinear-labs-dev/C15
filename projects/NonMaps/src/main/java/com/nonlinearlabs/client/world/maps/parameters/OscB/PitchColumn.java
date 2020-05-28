package com.nonlinearlabs.client.world.maps.parameters.OscB;

import com.nonlinearlabs.client.useCases.EditBufferUseCases;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.parameters.KeyTrackParameter;
import com.nonlinearlabs.client.world.maps.parameters.LabelModulationSource;
import com.nonlinearlabs.client.world.maps.parameters.ModulatableParameter;
import com.nonlinearlabs.client.world.maps.parameters.ModulateableNumericalControl;
import com.nonlinearlabs.client.world.maps.parameters.ModulationSourceHighPriority;
import com.nonlinearlabs.client.world.maps.parameters.ModulationSourceSlider;
import com.nonlinearlabs.client.world.maps.parameters.ParameterColumn;

class PitchColumn extends ParameterColumn {

	private class Pitch extends ModulatableParameter {

		private Pitch(MapsLayout parent) {
			super(parent, 83);
			addChild(new ModulateableNumericalControl(this, getParameterNumber()));
		}

		@Override
		public double getPadding() {
			return 2;
		}

		private boolean fine = false;

		@Override
		public Control mouseDown(Position eventPoint) {
			fine = eventPoint.getX() > getPixRect().getCenterPoint().getX();
			return super.mouseDown(eventPoint);
		}

		@Override
		public boolean forceFine(boolean f) {
			return fine;
		}

		@Override
		public Control doubleClick(Position pos) {
			if (pos.getX() < getPixRect().getCenterPoint().getX())
				setDefault();
			else {
				EditBufferUseCases.get().roundParameter(getParameterNumber());
			}
			return this;
		}
	}

	private class Keytracking extends ModulationSourceHighPriority {

		private Keytracking(MapsLayout parent) {
			super(parent, 85);
			addChild(new LabelModulationSource(this, getParameterNumber()));
			addChild(new KeyTrackParameter(this, getParameterNumber()));
		}

	}

	PitchColumn(MapsLayout parent) {
		super(parent);
		addChild(new Pitch(this));
		addChild(new ModulationSourceSlider(this, 86));
		addChild(new Keytracking(this));
	}
}
