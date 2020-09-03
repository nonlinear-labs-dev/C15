package com.nonlinearlabs.client.world.maps.parameters.OutputMixer;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.ColorTable;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.parameters.LabelModuleHeader;
import com.nonlinearlabs.client.world.maps.parameters.ParameterGroup;

public class OutputMixer extends ParameterGroup {

	public OutputMixer(MapsLayout parent) {
		super(parent, "Mixer");
		addChild(new LabelModuleHeader(this) {
			@Override
			public RGB getColorFont() {
				return RGB.lighterGray();
			}
		});
		addChild(new OutputMixerControls(this));
	}

	@Override
	public void draw(Context2d ctx, Context2d overlay, int invalidationMask) {
		getPixRect().fill(ctx, getColorModuleBackground());
		super.draw(ctx, overlay, invalidationMask);
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
}
