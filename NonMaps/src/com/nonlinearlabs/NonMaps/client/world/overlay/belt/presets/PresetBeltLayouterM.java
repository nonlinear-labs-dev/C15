package com.nonlinearlabs.NonMaps.client.world.overlay.belt.presets;

import com.nonlinearlabs.NonMaps.client.world.overlay.OverlayLayout;

public class PresetBeltLayouterM extends PresetBeltLayouter {

	public PresetBeltLayouterM(BeltPresetLayout layout) {
		super(layout);
	}

	@Override
	public boolean doLayout(double w, double h) {
		hide(layout.menu);

		double margin = OverlayLayout.getButtonDimension() / 2;
		double left = margin;
		double right = w - margin;
		double autoLoadWidth = layout.autoLoad.getSelectedImage().getImgWidth();
		double currentPresetWidth = layout.currentPreset.getDesiredWidth();
		double currentPresetHeight = layout.currentPreset.getDesiredHeight();

		layout.store.doLayout(left, 0, h);
		left += layout.store.getRelativePosition().getWidth() + margin;

		layout.autoLoad.doLayout(right - autoLoadWidth, 0, autoLoadWidth, h);
		right -= autoLoadWidth + margin;

		layout.currentPreset.doLayout(right - currentPresetWidth, (h - currentPresetHeight) / 2, currentPresetWidth, currentPresetHeight);
		right -= currentPresetWidth + margin;

		layout.load.doLayout(right - LoadButtonArea.getFixedWidth(), 0, h);
		right -= layout.load.getRelativePosition().getWidth() + margin;

		double bankWidth = right - left;
		if (bankWidth < getMinBankControlWidth())
			return false;

		layout.bank.doLayout(left, 0, bankWidth, h);

		return true;
	}
}
