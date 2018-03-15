package com.nonlinearlabs.NonMaps.client.world.overlay.belt.presets;

import com.google.gwt.canvas.dom.client.Context2d;
import com.google.gwt.canvas.dom.client.Context2d.TextAlign;
import com.nonlinearlabs.NonMaps.client.Millimeter;
import com.nonlinearlabs.NonMaps.client.world.Rect;
import com.nonlinearlabs.NonMaps.client.world.maps.presets.bank.preset.Preset;
import com.nonlinearlabs.NonMaps.client.world.overlay.Label;

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
		Preset p = getParent().getMapsPreset();

		if (p != null) {
			return p.getCurrentName();
		}

		return "";
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
