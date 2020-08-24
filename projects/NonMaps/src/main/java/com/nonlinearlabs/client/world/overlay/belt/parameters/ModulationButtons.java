package com.nonlinearlabs.client.world.overlay.belt.parameters;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.dataModel.setup.SetupModel;
import com.nonlinearlabs.client.presenters.EditBufferPresenterProvider;
import com.nonlinearlabs.client.presenters.ParameterPresenter;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.maps.parameters.PlayControls.MacroControls.Macros.MacroControls;
import com.nonlinearlabs.client.world.overlay.OverlayLayout;

public class ModulationButtons extends OverlayLayout {

	public ModulationButtons(BeltParameterLayout parent) {
		super(parent);

		MacroControls buttons[] = { MacroControls.A, MacroControls.B, MacroControls.C, MacroControls.D, MacroControls.E,
				MacroControls.F };

		for (MacroControls b : buttons) 
			addChild(new ModulationSourceButton(this, b));
	}

	@Override
	public void draw(Context2d ctx, int invalidationMask) {
		super.draw(ctx, invalidationMask);
		boolean changedIndicationEnabled = SetupModel.get().systemSettings.highlightChangedParameters.getBool();
		if (isChanged() && changedIndicationEnabled) {
			getPixRect().drawRoundedRect(ctx, Rect.ROUNDING_ALL, 5, 1.5, null, RGB.changedBeltBorder());
		}
	}

	private boolean isChanged() {
		ParameterPresenter p = EditBufferPresenterProvider.getPresenter().selectedParameter;
		return p.modulation.isModSourceChanged;
	}

	@Override
	public BeltParameterLayout getParent() {
		return (BeltParameterLayout) super.getParent();
	}

	@Override
	public void doLayout(double posx, double posy, double w, double h) {
		super.doLayout(posx, posy, w, h);
		double xSpacePerButton = w / 2;
		double ySpacePerButton = h / 3;
		double spacePerButton = Math.min(xSpacePerButton, ySpacePerButton);
		
		for (int y = 0; y < 3; y++) {
			for (int x = 0; x < 2; x++) {
				int index = y * 2 + x;
				getChildren().get(index).doLayout(x * spacePerButton, y * spacePerButton, spacePerButton,
				spacePerButton);
			}
		}
	}
}
