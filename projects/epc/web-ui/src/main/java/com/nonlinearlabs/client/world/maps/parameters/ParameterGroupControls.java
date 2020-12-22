package com.nonlinearlabs.client.world.maps.parameters;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.world.Gray;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.maps.LayoutResizingHorizontal;
import com.nonlinearlabs.client.world.maps.MapsLayout;

public class ParameterGroupControls extends LayoutResizingHorizontal {

	public ParameterGroupControls(MapsLayout parent) {
		super(parent);
	}

	@Override
	public void draw(Context2d ctx, Context2d overlay, Context2d menus, int invalidationMask) {
		Rect pixRect = getPixRect();
		pixRect.drawRoundedRect(ctx, Rect.ROUNDING_BOTTOM, toXPixels(8), toXPixels(2), new Gray(26), null);
		super.draw(ctx, overlay, menus, invalidationMask);
	}

	@Override
	public double getPadding() {
		return 2;
	}

	@Override
	public double getXMargin() {
		return 3;
	}
}
