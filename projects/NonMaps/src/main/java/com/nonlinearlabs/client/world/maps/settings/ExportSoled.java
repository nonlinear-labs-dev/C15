package com.nonlinearlabs.client.world.maps.settings;

import com.google.gwt.user.client.Window;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.maps.ContextMenu;
import com.nonlinearlabs.client.world.maps.NonPosition;

public class ExportSoled extends Setting {

	protected ExportSoled(DeveloperSettings parent) {
		super(parent, "Download Soled Screenshot", "Go!");
	}

	@Override
	public Control onValueButtonMouseDown(Position eventPoint) {
		NonMaps.get().getServerProxy().exportSoled();
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