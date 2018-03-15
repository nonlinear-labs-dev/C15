package com.nonlinearlabs.NonMaps.client.world.overlay.belt.presets;

import com.nonlinearlabs.NonMaps.client.world.overlay.OverlayLayout;

public class PresetBeltLayouterXS extends PresetBeltLayouter {

	public PresetBeltLayouterXS(BeltPresetLayout layout) {
		super(layout);
	}

	@Override
	public boolean doLayout(double w, double h) {
		hide(layout.menu);
		hide(layout.currentPreset);
		hide(layout.autoLoad);

		double margin = OverlayLayout.getButtonDimension() / 2;
		double left = margin;
		double right = w - margin;

		layout.store.doLayout(left, 0, h);
		left += layout.store.getRelativePosition().getWidth() + margin;

		layout.load.doLayout(right - LoadButtonArea.getFixedWidth(), 0, h);
		right -= layout.load.getRelativePosition().getWidth() + margin;

		double bankWidth = right - left;
		if (bankWidth < getMinBankControlWidth())
			return false;

		layout.bank.doLayout(left, 0, bankWidth, h);

		return true;
	}
}
