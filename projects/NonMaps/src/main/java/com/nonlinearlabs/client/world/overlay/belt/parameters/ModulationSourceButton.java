package com.nonlinearlabs.client.world.overlay.belt.parameters;

import com.nonlinearlabs.client.dataModel.editBuffer.ModulateableParameterModel.ModSource;
import com.nonlinearlabs.client.presenters.EditBufferPresenterProvider;
import com.nonlinearlabs.client.presenters.ParameterPresenter;
import com.nonlinearlabs.client.useCases.EditBufferUseCases;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.maps.parameters.PlayControls.MacroControls.Macros.MacroControls;
import com.nonlinearlabs.client.world.overlay.SVGImage;

public class ModulationSourceButton extends SVGImage {

	MacroControls which;

	public ModulationSourceButton(ModulationButtons parent, MacroControls which) {
		super(parent, "MC_" + which.name() + "_Enabled.svg", "MC_" + which.name() + "_Disabled.svg",
				"MC_" + which.name() + "_Deselected.svg");
		this.which = which;
	}

	@Override
	public ModulationButtons getParent() {
		return (ModulationButtons) super.getParent();
	}

	@Override
	public Control mouseDown(Position eventPoint) {
		if (isVisible() && getParent().isVisible()) {
			ParameterPresenter p = EditBufferPresenterProvider.getPresenter().selectedParameter;

			if (p.modulation.isModulateable) {
				if (MacroControls.from(p.modulation.modulationSource) == which)
					EditBufferUseCases.get().setModulationSource(p.id, ModSource.None);
				else
					EditBufferUseCases.get().setModulationSource(p.id, which.toModSource());
			}
			return this;
		}
		return null;
	}

	@Override
	public Control doubleClick(Position pos) {
		if (isVisible() && getParent().isVisible())
			return this;

		return null;
	}

	@Override
	public int getSelectedPhase() {
		ParameterPresenter p = EditBufferPresenterProvider.getPresenter().selectedParameter;

		if (p.modulation.isModulateable) {
			if (p.modulation.modulationSource != p.modulation.ogModSource)
				if (p.modulation.ogModSource == which.toModSource())
					return 2;

			return p.modulation.modulationSource == which.toModSource() ? 0 : 1;
		}

		return 0;
	}
}
