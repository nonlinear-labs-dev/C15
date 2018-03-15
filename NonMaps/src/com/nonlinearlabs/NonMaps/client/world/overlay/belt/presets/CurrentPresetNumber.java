package com.nonlinearlabs.NonMaps.client.world.overlay.belt.presets;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.NonMaps.client.Millimeter;
import com.nonlinearlabs.NonMaps.client.NonMaps;
import com.nonlinearlabs.NonMaps.client.world.RGB;
import com.nonlinearlabs.NonMaps.client.world.Rect;
import com.nonlinearlabs.NonMaps.client.world.overlay.Label;
import com.nonlinearlabs.NonMaps.client.world.overlay.OverlayLayout;

public class CurrentPresetNumber extends Label {

	public CurrentPresetNumber(OverlayLayout parent) {
		super(parent);
		super.setFontColor(new RGB(120, 120, 120));
		super.setFontHeightInMM(4.5);
	}

	@Override
	public String getDrawText(Context2d ctx) {
		String text = NonMaps.theMaps.getNonLinearWorld().getPresetManager().getLoadedPresetNumberString();
		if (NonMaps.theMaps.getNonLinearWorld().getParameterEditor().isAnyParameterLocked())
			text += "\t L";

		return text;
	}

	public double getDesiredWidth() {
		return Millimeter.toPixels(25);
	}

	public double getDesiredHeight() {
		return Millimeter.toPixels(20);
	}

	@Override
	protected double getVerticalFontDisplacement() {
		return Millimeter.toPixels(4);
	}

	@Override
	protected RGB getColorFontForSplit(int i) {
		if (i == 0)
			return getLineColor();

		return new RGB(0, 0, 0);
	}

	@Override
	public void drawSplit(int idx, Context2d ctx, String split, double x, double y) {
		if (idx == 1) {
			Rect r = new Rect(x + Millimeter.toPixels(1), getPixRect().getCenterPoint().getY() + getVerticalFontDisplacement()
					- fontHeightInPx / 2, Millimeter.toPixels(3), fontHeightInPx);
			r.fill(ctx, new RGB(255, 255, 255));
		}
		super.drawSplit(idx, ctx, split, x, y);
	}
}
