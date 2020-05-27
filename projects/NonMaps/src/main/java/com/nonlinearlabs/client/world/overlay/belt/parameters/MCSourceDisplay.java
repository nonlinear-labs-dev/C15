package com.nonlinearlabs.client.world.overlay.belt.parameters;

import com.nonlinearlabs.client.presenters.EditBufferPresenterProvider;
import com.nonlinearlabs.client.presenters.ParameterPresenter;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.overlay.SVGImage;

public class MCSourceDisplay extends SVGImage {

	public MCSourceDisplay(BeltParameterLayout parent) {
		super(parent, "MC_A_Label.svg", "MC_B_Label.svg", "MC_C_Label.svg", "MC_D_Label.svg", "MC_E_Label.svg", "MC_F_Label.svg");
	}

	@Override
	public BeltParameterLayout getParent() {
		return (BeltParameterLayout) super.getParent();
	}

	@Override
	public int getSelectedPhase() {
		ParameterPresenter presenter = EditBufferPresenterProvider.getPresenter().selectedParameter;

		if (presenter.modulation.isModulateable) {

			switch (presenter.modulation.modulationSource) {
			case A:
				return 0;
			case B:
				return 1;
			case C:
				return 2;
			case D:
				return 3;
			case E:
				return 4;
			case F:
				return 5;
			default:
				break;
			}
		}

		return -1;
	}

	@Override
	public Control mouseDown(Position eventPoint) {
		if (getParent().isOneOf(BeltParameterLayout.Mode.mcValue, BeltParameterLayout.Mode.mcAmount,
				BeltParameterLayout.Mode.mcSource, BeltParameterLayout.Mode.paramValue))
			getParent().setMode(BeltParameterLayout.Mode.modulateableParameter);
		else if (getSelectedPhase() != -1)
			getParent().setMode(BeltParameterLayout.Mode.paramValue);

		return this;
	}
}
