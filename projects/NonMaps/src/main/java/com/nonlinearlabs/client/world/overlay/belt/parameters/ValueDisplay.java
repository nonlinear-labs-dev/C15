package com.nonlinearlabs.client.world.overlay.belt.parameters;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.Millimeter;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.overlay.Label;

public class ValueDisplay extends Label {

	public ValueDisplay(BeltParameterLayout parent) {
		super(parent);
		super.setFontHeightInMM(6);
	}

	@Override
	public BeltParameterLayout getParent() {
		return (BeltParameterLayout) super.getParent();
	}

	public double calculateTextWidth() {
		Context2d ctx = NonMaps.get().getCanvas().getContext2d();
		String text = getDrawText(ctx);
		ctx.setFont(getFontHeight(getPixRect()) + "px 'SSP-LW25'");
		return ctx.measureText(text).getWidth();
	}

	@Override
	public String getDrawText(Context2d ctx) {
		String[] str = getParent().getDecoratedValue(true);

		for (String s : str)
			if (ctx.measureText(s).getWidth() < getPixRect().getWidth() * 0.8)
				return s;

		return "";
	}

	@Override
	public void draw(Context2d ctx, int invalidationMask) {
		if (getPixRect().getWidth() >= Millimeter.toPixels(20))
			super.draw(ctx, invalidationMask);
	}

	@Override
	public Control wheel(Position eventPoint, double amount, boolean fine) {
		return getParent().wheel(eventPoint, amount, fine);
	}
}
