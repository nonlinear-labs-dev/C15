package com.nonlinearlabs.client.world.overlay.belt.presets;
import com.nonlinearlabs.client.Millimeter;

public class PresetBeltLayouterS extends PresetBeltLayouter {

	public PresetBeltLayouterS(BeltPresetLayout layout) {
		super(layout);
	}

	@Override
	public boolean doLayout(double w, double h) {
		hide(layout.menu);
		hide(layout.currentPreset);

		double margin = getDefaultMargin();
		double left = margin;
		double right = w - margin;
		double autoLoadWidth = Millimeter.toPixels(25);

		layout.store.doLayout(left, 0, h);
		left += layout.store.getRelativePosition().getWidth();

		layout.autoLoad.doLayout(right - autoLoadWidth, 0, autoLoadWidth, h);
		right -= autoLoadWidth;

		layout.load.doLayout(right, 0, h);
		right -= layout.load.getRelativePosition().getWidth() + margin;

		double bankWidth = right - left;
		if (bankWidth < getMinBankControlWidth())
			return false;

		layout.bank.doLayout(left, 0, bankWidth, h);

		super.layoutLockSymbol(w);
		return true;
	}
}
