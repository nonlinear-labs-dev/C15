package com.nonlinearlabs.client.world.overlay.belt.presets;

import com.google.gwt.canvas.dom.client.Context2d;
import com.google.gwt.canvas.dom.client.Context2d.TextAlign;
import com.nonlinearlabs.client.Millimeter;
import com.nonlinearlabs.client.presenters.PresetPresenterProviders;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.overlay.Label;

public class PresetName extends Label {

	public PresetName(BeltPreset parent) {
		super(parent);
	}

	@Override
	public BeltPreset getParent() {
		return (BeltPreset) super.getParent();
	}

	@Override
	public String getDrawText(Context2d ctx) {
		var p = PresetPresenterProviders.get().getPresenter(getParent().getUuid());
		// TODO: does the name have the suffix?
		return p.name;
	}

	protected TextAlign getAlignment() {
		return TextAlign.LEFT;
	}

	@Override
	protected Rect getTextRect() {
		Rect r = super.getTextRect().copy();
		r.applyPadding(0, 0, Millimeter.toPixels(1), 0);
		return r;
	}

}
