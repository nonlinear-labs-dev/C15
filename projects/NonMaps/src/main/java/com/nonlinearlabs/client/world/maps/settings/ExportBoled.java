package com.nonlinearlabs.client.world.maps.settings;

import com.google.gwt.user.client.Window;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.maps.ContextMenu;
import com.nonlinearlabs.client.world.maps.NonPosition;

public class ExportBoled extends Setting {

	protected ExportBoled(DeveloperSettings parent) {
		super(parent, "Download Boled Screenshot", "Go!");
	}

	@Override
	public Control onValueButtonMouseDown(Position eventPoint) {
        NonMaps.get().getServerProxy().exportBoled();
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