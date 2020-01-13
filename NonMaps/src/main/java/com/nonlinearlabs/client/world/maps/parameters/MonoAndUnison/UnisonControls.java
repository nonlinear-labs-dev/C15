package com.nonlinearlabs.client.world.maps.parameters.MonoAndUnison;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.world.Gray;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.parameters.LabelModuleHeader;
import com.nonlinearlabs.client.world.maps.parameters.ParameterGroup;

class UnisonControls extends ParameterGroup {

	UnisonControls(MapsLayout parent) {
		super(parent, "Unison");
		addChild(new LabelModuleHeader(this) {
			@Override
			public RGB getColorFont() {
				return RGB.lighterGray();
			}

			@Override
			public void drawBackground(Context2d ctx, Rect pixRect) {
				pixRect.drawRoundedRect(ctx, Rect.ROUNDING_NONE, toXPixels(6), toXPixels(2), new Gray(87), null);
			}
		});
		addChild(new UnisonColumn(this));
	}

	@Override
	public void drawBorder(Context2d ctx) {
		return;
	}
}
