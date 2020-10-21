package com.nonlinearlabs.client.world.maps.presets.bank.preset;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.parameters.ZoomReactingControl;

public class ColorTag extends ZoomReactingControl {

	public ColorTag(MapsLayout parent) {
		super(parent);
	}

	protected RGB calcColor() {
		return RGB.red();// Color.toEnum(getParent().getAttribute("color")).toRGB();
	}

	@Override
	protected double getBasicWidth() {
		return 6;
	}

	@Override
	protected double getBasicHeight() {
		return 30;
	}

	@Override
	protected double getMinHeight() {
		return 0;
	}

	@Override
	public void draw(Context2d ctx, Context2d overlay, int invalidationMask) {
		RGB color = calcColor();
		if (color == null)
			return;

		Rect r = getPixRect().copy();
		r.reduceHeightBy(r.getHeight() / 3);
		r.fill(ctx, color);
	}
}
