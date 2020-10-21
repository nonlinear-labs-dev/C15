package com.nonlinearlabs.client.world.overlay;

import com.nonlinearlabs.client.Millimeter;
import com.nonlinearlabs.client.useCases.PresetManagerUseCases;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;

public class PresetContextMenuItem extends ContextMenuItem {

	public PresetContextMenuItem(ContextMenu presetContextMenu, String text) {
		super(presetContextMenu, text);
	}

	private void endMultiplePresetSelection() {
		PresetManagerUseCases.get().finishMultipleSelection();
	}

	@Override
	public Control click(Position eventPoint) {
		endMultiplePresetSelection();
		return super.click(eventPoint);
	}

	@Override
	public double getDesiredHeight() {
		return Millimeter.toPixels(10);
	}

	@Override
	public double getDesiredWidth() {
		return Millimeter.toPixels(17);
	}

}
