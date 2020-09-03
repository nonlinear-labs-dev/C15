package com.nonlinearlabs.client.world.maps.presets.bank.preset;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.RGBA;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.parameters.ZoomReactingControl;

public class ColorTag extends ZoomReactingControl {

	public enum Color {
		green, blue, yellow, orange, purple, red, none;

		public RGB toRGB() {
			switch (this) {
				case green:
					return new RGB(82, 173, 69);
				case blue:
					return new RGB(75, 105, 213);
				case yellow:
					return new RGB(204, 197, 54);
				case orange:
					return new RGB(221, 149, 55);
				case purple:
					return new RGB(183, 95, 174);
				case red:
					return new RGB(207, 43, 59);
				case none:
				default:
					return new RGBA(0, 0, 0, 0);
			}
		}

		static public Color toEnum(String s) {
			if (s == "")
				return none;
			else
				return Color.valueOf(s);
		}
	}

	public ColorTag(MapsLayout parent) {
		super(parent);
	}

	protected RGB calcColor() {
		return Color.toEnum(getParent().getAttribute("color")).toRGB();
	}

	@Override
	public Preset getParent() {
		return (Preset) super.getParent();
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
