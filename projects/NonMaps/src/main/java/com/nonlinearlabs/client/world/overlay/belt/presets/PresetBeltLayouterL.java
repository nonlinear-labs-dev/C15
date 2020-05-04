package com.nonlinearlabs.client.world.overlay.belt.presets;

import com.nonlinearlabs.client.Millimeter;

public class PresetBeltLayouterL extends PresetBeltLayouter {

	public PresetBeltLayouterL(BeltPresetLayout layout) {
		super(layout);
	}

	@Override
	public boolean doLayout(double w, double h) {
		double margin = getDefaultMargin();
		double left = margin;
		double right = w - margin;
		double autoLoadWidth = Millimeter.toPixels(25);
		double currentPresetWidth = layout.currentPreset.getDesiredWidth();
		double currentPresetHeight = layout.currentPreset.getDesiredHeight();

		double menuY = h / 6;
		layout.menu.doLayout(left, menuY, h - menuY - menuY, false);
		left += layout.menu.getRelativePosition().getWidth() + margin;

		layout.store.doLayout(left, 0, h);
		left += layout.store.getRelativePosition().getWidth() + margin;

		layout.autoLoad.doLayout(right - autoLoadWidth, 0, autoLoadWidth, h);
		right -= autoLoadWidth + margin;

		layout.currentPreset.doLayout(right - currentPresetWidth, (h - currentPresetHeight) / 2, currentPresetWidth,
				currentPresetHeight);

		right -= currentPresetWidth + margin;

		layout.load.doLayout(right, 0, h);
		right -= layout.load.getRelativePosition().getWidth() + margin;

		double bankWidth = right - left;
		if (bankWidth < getMinBankControlWidth())
			return false;

		layout.bank.doLayout(left, 0, bankWidth, h);
		right = layout.bank.getRelativePosition().getRight() + margin;

		layout.load.getRelativePosition().moveTo(right, layout.load.getRelativePosition().getTop());
		right += layout.load.getRelativePosition().getWidth() + margin;
		layout.currentPreset.getRelativePosition().moveTo(right, layout.currentPreset.getRelativePosition().getTop());
		right += layout.currentPreset.getRelativePosition().getWidth() / 2 - Millimeter.toPixels(5);

		super.layoutLockSymbol(w);
		return true;
	}

}
