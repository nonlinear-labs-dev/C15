package com.nonlinearlabs.client.world.maps.parameters.Cabinet;

import com.nonlinearlabs.client.ColorTable;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.parameters.LabelModuleHeader;
import com.nonlinearlabs.client.world.maps.parameters.ParameterGroup;

public class Cabinet extends ParameterGroup {

	public Cabinet(MapsLayout parent) {
		super(parent, "Cab");
		addChild(new LabelModuleHeader(this));
		addChild(new CabinetControls(this));
	}

	@Override
	public RGB getColorSliderHighlight() {
		return ColorTable.getColorFXHeaderBackground();
	}

	@Override
	public RGB getColorSliderBackground() {
		return ColorTable.getColorSliderBackground();
	}

	@Override
	public RGB getColorModuleBackground() {
		return ColorTable.getColorFXBackground();
	}

	@Override
	public RGB getColorModuleHeaderBackground() {
		return ColorTable.getColorFXHeaderBackground();
	}

	@Override
	public RGB getColorFontValueDisplay() {
		return ColorTable.getColorFXHeaderBackground();
	}

	@Override
	public RGB getColorFontValueDisplaySelected() {
		return ColorTable.getColorFXHeaderBackground();
	}

	@Override
	public RGB getColorObjectBackgroundSelected() {
		return ColorTable.getColorFXSelectedBackground();
	}

	@Override
	public RGB getColorObjectContourSelected() {
		return ColorTable.getColorFXHeaderBackground();
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
