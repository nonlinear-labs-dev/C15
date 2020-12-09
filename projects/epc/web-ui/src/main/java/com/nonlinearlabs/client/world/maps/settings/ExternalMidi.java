package com.nonlinearlabs.client.world.maps.settings;

import com.nonlinearlabs.client.presenters.DeviceSettingsProvider;
import com.nonlinearlabs.client.useCases.EditBufferUseCases;
import com.nonlinearlabs.client.world.NonLinearWorld;
import com.nonlinearlabs.client.world.maps.IContextMenu;
import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.NonPosition;
import com.nonlinearlabs.client.world.maps.settings.OnOffContextMenu.Items;
import com.nonlinearlabs.client.world.maps.settings.OnOffContextMenu.Listener;

public class ExternalMidi extends Setting {

	protected ExternalMidi(MapsLayout parent) {
		super(parent, "Enable External Midi", "Off");
	
		DeviceSettingsProvider.get().register(t -> {
			setCurrentValue(t.externalMidiEnabled);
			return true;
		});
    }

	@Override
	public IContextMenu createMenu(NonPosition pos) {
		NonLinearWorld world = getWorld();
		return world.setContextMenu(new OnOffContextMenu(new Listener() {

			@Override
			public void setValue(Items item) {
				EditBufferUseCases.get().setExternalMidiSetting(item == Items.ON);
			}
		}, world), pos);
	}

	@Override
	public void setDefault() {
		setCurrentValue(Items.OFF.toString());
	}

	public void setCurrentValue(boolean val) {
		super.setCurrentValue(val ? "On" : "Off");
		invalidate(INVALIDATION_FLAG_UI_CHANGED);
	}
}
