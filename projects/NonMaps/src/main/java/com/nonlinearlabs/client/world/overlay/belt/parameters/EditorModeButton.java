package com.nonlinearlabs.client.world.overlay.belt.parameters;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.dataModel.setup.SetupModel;
import com.nonlinearlabs.client.presenters.EditBufferPresenterProvider;
import com.nonlinearlabs.client.presenters.ParameterPresenter;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.overlay.SVGImage;

public class EditorModeButton extends SVGImage {

	public EditorModeButton(BeltParameterLayout parent) {
		super(parent, "MC_Edit_Active.svg", "MC_Edit_Enabled.svg");
	}

	@Override
	public void draw(Context2d ctx, int invalidationMask) {
		super.draw(ctx, invalidationMask);
		boolean changedIndicationEnabled = SetupModel.get().systemSettings.highlightChangedParameters.getBool();
		if (isChanged() && changedIndicationEnabled) {
			Rect pix = getPixRect().copy();
			pix = pix.getReducedBy(pix.getWidth() * -0.5);
			pix.drawRoundedRect(ctx, Rect.ROUNDING_ALL, 5, 1.5, null, RGB.changedBeltBorder());
		}
	}

	private boolean isChanged() {
		ParameterPresenter p = EditBufferPresenterProvider.getPresenter().selectedParameter;
		return p.modulation.isModAmountChanged || p.modulation.isModSourceChanged || p.modulation.isMCPosChanged;
	}

	@Override
	public BeltParameterLayout getParent() {
		return (BeltParameterLayout) super.getParent();
	}

	@Override
	public Control mouseDown(Position eventPoint) {
		if (isActive())
			getParent().setMode(BeltParameterLayout.Mode.modulateableParameter);
		else
			getParent().setMode(BeltParameterLayout.Mode.paramValue);

		return this;
	}

	@Override
	public boolean isVisible() {
		if (getParent().isOneOf(BeltParameterLayout.Mode.unmodulateableParameter))
			return false;

		return super.isVisible();
	}

	@Override
	public int getSelectedPhase() {
		boolean active = isActive();
		return active ? 0 : 1;
	}

	private boolean isActive() {
		return getParent().isOneOf(BeltParameterLayout.Mode.mcValue, BeltParameterLayout.Mode.mcAmount,
				BeltParameterLayout.Mode.mcSource, BeltParameterLayout.Mode.paramValue,
				BeltParameterLayout.Mode.mcLower, BeltParameterLayout.Mode.mcUpper);
	}

	@Override
	public Control doubleClick(Position pos) {
		return this;
	}
}
