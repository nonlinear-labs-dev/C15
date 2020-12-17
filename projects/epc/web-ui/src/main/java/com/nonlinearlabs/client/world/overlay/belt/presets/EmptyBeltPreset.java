package com.nonlinearlabs.client.world.overlay.belt.presets;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.world.Gray;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.overlay.Label;
import com.nonlinearlabs.client.world.overlay.OverlayLayout;

public class EmptyBeltPreset extends OverlayLayout {

	private class EmptyLabel extends Label {

		public EmptyLabel(EmptyBeltPreset parent) {
			super(parent);
		}

		@Override
		public String getDrawText(Context2d ctx) {
			return "- empty -";
		}

		@Override
		public void draw(Context2d ctx, Context2d overlay, int invalidationMask) {
			Rect rect = getPixRect();
			rect.fill(ctx, new Gray(25));
			super.draw(ctx, overlay, invalidationMask);
		}

		@Override
		public RGB getColorFont() {
			return new Gray(128);
		}
	}

	Label emptyLabel;

	public EmptyBeltPreset(OverlayLayout parent) {
		super(parent);
		emptyLabel = addChild(new EmptyLabel(this));
	}

	@Override
	public void doLayout(double x, double y, double w, double h) {
		super.doLayout(x, y, w, h);
		emptyLabel.doLayout(0, 0, w, h);
	}

}