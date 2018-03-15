package com.nonlinearlabs.NonMaps.client.world.overlay.belt.presets;

import com.nonlinearlabs.NonMaps.client.world.overlay.OverlayLayout;

public class PresetBeltLayouterXXS extends PresetBeltLayouter {

	public PresetBeltLayouterXXS(BeltPresetLayout layout) {
		super(layout);
	}

	@Override
	public boolean doLayout(double w, double h) {
		hide(layout.menu);
		hide(layout.store);
		hide(layout.currentPreset);
		hide(layout.autoLoad);

		double margin = OverlayLayout.getButtonDimension() / 2;
		double loadButtonsWidth = LoadButtonArea.getFixedWidth();

		layout.bank.doLayout(margin, 0, w - margin - loadButtonsWidth - margin - margin, h);
		layout.load.doLayout(w - margin - loadButtonsWidth, 0, h);

		return true;
	}
}
