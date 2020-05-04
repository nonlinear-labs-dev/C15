package com.nonlinearlabs.client.world.maps.settings;

import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.maps.ContextMenu;
import com.nonlinearlabs.client.world.maps.NonPosition;

public class TestHWUIDisplay extends Setting {

	protected TestHWUIDisplay(DeveloperSettings parent) {
		super(parent, "Test HWUI Displays", "Next!");
	}

	@Override
	public Control onValueButtonMouseDown(Position eventPoint) {
		NonMaps.theMaps.getServerProxy().incDisplayTest();
		return this;
	}

	@Override
	public ContextMenu createMenu(NonPosition pos) {
		return null;
	}

	@Override
	public void setDefault() {

	}

}