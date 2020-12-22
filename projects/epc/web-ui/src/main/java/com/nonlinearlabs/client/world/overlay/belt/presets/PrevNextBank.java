package com.nonlinearlabs.client.world.overlay.belt.presets;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Gray;
import com.nonlinearlabs.client.world.overlay.SVGImage;

public abstract class PrevNextBank extends SVGImage {

	public PrevNextBank(Control parent, String... imageNames) {
		super(parent, imageNames);
	}

	@Override
	protected double getPadding() {
		return 4;
	}

	@Override
	public void draw(Context2d ctx, Context2d overlay, Context2d menus, int invalidationMask) {
		getPixRect().fill(ctx, new Gray(77));
		super.draw(ctx, overlay, menus, invalidationMask);
	}

}