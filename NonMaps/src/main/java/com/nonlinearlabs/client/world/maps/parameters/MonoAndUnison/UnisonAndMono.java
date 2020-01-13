package com.nonlinearlabs.client.world.maps.parameters.MonoAndUnison;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.ColorTable;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.maps.LayoutResizingVertical;
import com.nonlinearlabs.client.world.maps.MapsLayout;

public class UnisonAndMono extends LayoutResizingVertical {

	public UnisonAndMono(MapsLayout parent) {
		super(parent);
		addChild(new UnisonAndMonoModuleHeader(this));
		addChild(new UnisonAndMonoControls(this));
	}

	@Override
	public RGB getColorSliderHighlight() {
		return RGB.lightGray();
	}

	@Override
	public RGB getColorSliderBackground() {
		return ColorTable.getColorSliderBackground();
	}

	@Override
	public RGB getColorModuleBackground() {
		return ColorTable.getColorAUXBackground();
	}

	@Override
	public RGB getColorModuleHeaderBackground() {
		return RGB.lightGray();
	}

	@Override
	public RGB getColorFontValueDisplay() {
		return RGB.lightGray();
	}

	@Override
	public RGB getColorFontValueDisplaySelected() {
		return RGB.lightGray();
	}

	@Override
	public RGB getColorObjectBackgroundSelected() {
		return ColorTable.getColorAUXSelectedBackground();
	}

	@Override
	public RGB getColorObjectContourSelected() {
		return RGB.lightGray();
	}

	@Override
	public RGB getColorObjectContour() {
		return ColorTable.getColorContour();
	}

	@Override
	public RGB getColorIndicator() {
		return ColorTable.getColorIndicator();
	}

	protected void fillBackground(Context2d ctx) {
		getPixRect().drawRoundedRect(ctx, Rect.ROUNDING_ALL, toXPixels(5), toXPixels(2), getColorModuleBackground(), getColorObjectContour());
	}

	@Override
	public void draw(Context2d ctx, int invalidationMask) {
		fillBackground(ctx);
		super.draw(ctx, invalidationMask);
		getPixRect().drawRoundedRect(ctx, Rect.ROUNDING_ALL, toXPixels(6), toXPixels(2), null,
				getColorModuleHeaderBackground());
	}
}
