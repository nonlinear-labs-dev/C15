package com.nonlinearlabs.client.world.overlay.belt.parameters;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.Millimeter;
import com.nonlinearlabs.client.dataModel.setup.SetupModel;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.overlay.SVGImage;
import com.nonlinearlabs.client.world.overlay.belt.parameters.BeltParameterLayout.Mode;

abstract class MCRadioButton extends SVGImage {
	Mode mode;

	MCRadioButton(BeltParameterLayout parent, Mode m, String... imageNames) {
		super(parent, imageNames);
		this.mode = m;
	}

	@Override
	public BeltParameterLayout getParent() {
		return (BeltParameterLayout) super.getParent();
	}

	abstract public boolean isChanged();

	@Override
	public void draw(Context2d ctx, int invalidationMask) {
		super.draw(ctx, invalidationMask);
		boolean changedIndicationEnabled = SetupModel.get().systemSettings.highlightChangedParameters.getBool();
		if (isChanged() && changedIndicationEnabled) {
			Rect pix = getPixRect().copy();
			pix = pix.getReducedBy(pix.getWidth() * 0.2);
			pix.drawRoundedRect(ctx, Rect.ROUNDING_ALL, 5, 1.5, null, RGB.changedBeltBorder());
		}
	}

	@Override
	public boolean isVisible() {
		if (super.isVisible()) {
			return getPixRect().getWidth() > Millimeter.toPixels(10);
		}
		return false;
	}

	@Override
	public Control click(Position eventPoint) {
		getParent().toggleMcEditMode(this.mode);
		return this;
	}

	@Override
	public Control doubleClick(Position pos) {
		return this;
	}

	@Override
	public int getSelectedPhase() {
		return getParent().getMode() == this.mode ? 0 : 1;
	}
}
