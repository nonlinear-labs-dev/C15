package com.nonlinearlabs.client.world.overlay.belt.presets;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.presenters.PresetPresenterProviders;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.overlay.OverlayControl;

public class PresetColorTag extends OverlayControl {

	public PresetColorTag(BeltPreset parent) {
		super(parent);
	}

	@Override
	public BeltPreset getParent() {
		return (BeltPreset) super.getParent();
	}

	protected RGB calcColor() {
		var p = PresetPresenterProviders.get().getPresenter(getParent().getUuid());
		return p.color.toRGB();
	}

	@Override
	public void draw(Context2d ctx, Context2d overlay, Context2d menus, int invalidationMask) {
		Rect pixRect = getPixRect().copy();
		double padding = getRelativePosition().getHeight() / 10;
		pixRect.applyPadding(0, padding, 0, padding);
		pixRect.fill(ctx, calcColor());
	}
}
