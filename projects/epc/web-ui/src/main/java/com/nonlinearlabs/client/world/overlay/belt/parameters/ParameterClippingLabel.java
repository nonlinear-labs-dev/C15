package com.nonlinearlabs.client.world.overlay.belt.parameters;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.presenters.EditBufferPresenterProvider;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.overlay.Label;
import com.nonlinearlabs.client.world.overlay.OverlayLayout;
import com.nonlinearlabs.client.world.overlay.belt.parameters.BeltParameterLayout.Mode;

public class ParameterClippingLabel extends Label {
	BeltParameterLayout.Mode mMode;

	ParameterClippingLabel(OverlayLayout parent, BeltParameterLayout.Mode mode) {
		super(parent);
		setFontHeightInMM(6);
		mMode = mode;
	}

	public boolean isClipping() {
		if (mMode == Mode.mcLower)
			return EditBufferPresenterProvider.getPresenter().selectedParameter.modulation.lowerClipping;

		return EditBufferPresenterProvider.getPresenter().selectedParameter.modulation.upperClipping;
	}

	@Override
	public String getDrawText(Context2d ctx) {
		return isClipping() ? "!" : "";
	}

	@Override
	public void draw(Context2d ctx, Context2d overlay, Context2d menus, int invalidationMask) {
		if (isClipping()) {
			getPixRect().fill(ctx, new RGB(51, 51, 51));
			super.draw(ctx, overlay, menus, invalidationMask);
		}
	}

}
