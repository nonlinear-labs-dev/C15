package com.nonlinearlabs.NonMaps.client.world.overlay.belt.presets;

import com.nonlinearlabs.NonMaps.client.Millimeter;
import com.nonlinearlabs.NonMaps.client.world.overlay.OverlayLayout;
import com.nonlinearlabs.NonMaps.client.world.overlay.SVGImage;

public class LoadButtonArea extends OverlayLayout {

	private PrevNextPresetButtons prevNext;
	private LoadPreset load;
	AdvancedBankInformation bankInfos;

	protected LoadButtonArea(BeltPresetLayout parent) {
		super(parent);

		addChild(bankInfos = new AdvancedBankInformation(this));
		addChild(prevNext = new PrevNextPresetButtons(this));
		addChild(load = new LoadPreset(this));
	}

	public void doLayout(double x, double y, double h) {
		double w = getFixedWidth();
		double margin = getMargin();

		double buttonWidth = SVGImage.calcSVGDimensionToPixels(36);
		double buttonHeight = SVGImage.calcSVGDimensionToPixels(34);

		bankInfos.doLayout(0, 0, buttonWidth, Millimeter.toPixels(12));
		prevNext.doLayout(0, Millimeter.toPixels(12), buttonWidth, h - Millimeter.toPixels(12));
		load.doLayout(prevNext.getRelativePosition().getRight() + margin, Millimeter.toPixels(18), buttonWidth, buttonHeight);

		super.doLayout(x, y, w, h);
	}

	static public double getFixedWidth() {
		double buttonWidth = SVGImage.calcSVGDimensionToPixels(36);
		double margin = getMargin();
		return buttonWidth + margin + buttonWidth;
	}

	public static double getMargin() {
		return getButtonDimension() / 2;
	}

	public PrevNextPresetButtons getPrevNext() {
		return prevNext;
	}

}
