package com.nonlinearlabs.client.world.overlay;

import com.nonlinearlabs.client.Millimeter;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.maps.presets.PresetManager;

public class PresetContextMenuItem extends ContextMenuItem {

	public PresetContextMenuItem(ContextMenu presetContextMenu, String text) {
		super(presetContextMenu, text);
	}

	private void endMultiplePresetSelection() {
		PresetManager pm = NonMaps.get().getNonLinearWorld().getPresetManager();
		pm.closeMultiSelection();
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

}
