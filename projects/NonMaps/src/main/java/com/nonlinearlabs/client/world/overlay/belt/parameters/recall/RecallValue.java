package com.nonlinearlabs.client.world.overlay.belt.parameters.recall;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.Millimeter;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.world.Gray;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.overlay.Label;
import com.nonlinearlabs.client.world.overlay.OverlayLayout;

public abstract class RecallValue extends Label {

	public RecallValue(OverlayLayout parent) {
		super(parent);
		setFontHeightInMM(5);
		setFontColor(new Gray(155));
	}

	public double getFontSizeInPixel() {
		return getFontHeight(getPixRect());
	}

	public double calculateTextWidth() {
		Context2d ctx = NonMaps.get().getCanvas().getContext2d();
		String text = getDrawText(ctx);
		ctx.setFont(getFontSizeInPixel() + "px 'SSP-LW25'");
		return ctx.measureText(text).getWidth();
	}

	@Override
	public void draw(Context2d ctx, int invalidationMask) {
		getPixRect().drawRoundedRect(ctx, Rect.ROUNDING_ALL, Millimeter.toPixels(1), 1, new Gray(77),
				new Gray(77).brighter(15));
		super.draw(ctx, invalidationMask);
	}
}