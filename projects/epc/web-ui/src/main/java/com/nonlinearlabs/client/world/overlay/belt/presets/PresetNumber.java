package com.nonlinearlabs.client.world.overlay.belt.presets;

import com.google.gwt.canvas.dom.client.Context2d;
import com.google.gwt.canvas.dom.client.Context2d.TextAlign;
import com.nonlinearlabs.client.presenters.PresetPresenterProviders;
import com.nonlinearlabs.client.world.overlay.Label;

public class PresetNumber extends Label {

	public PresetNumber(BeltPreset parent) {
		super(parent);
	}

	@Override
	public BeltPreset getParent() {
		return (BeltPreset) super.getParent();
	}

	@Override
	public String getDrawText(Context2d ctx) {
		var p = PresetPresenterProviders.get().getPresenter(getParent().getUuid());
		return p.paddedNumber;
	}

	protected TextAlign getAlignment() {
		return TextAlign.RIGHT;
	}
}
