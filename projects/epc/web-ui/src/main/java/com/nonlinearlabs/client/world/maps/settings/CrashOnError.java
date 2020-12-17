package com.nonlinearlabs.client.world.maps.settings;

import java.util.function.Function;

import com.nonlinearlabs.client.dataModel.setup.SetupModel;
import com.nonlinearlabs.client.dataModel.setup.SetupModel.BooleanValues;
import com.nonlinearlabs.client.useCases.SystemSettings;
import com.nonlinearlabs.client.world.NonLinearWorld;
import com.nonlinearlabs.client.world.maps.IContextMenu;
import com.nonlinearlabs.client.world.maps.NonPosition;
import com.nonlinearlabs.client.world.maps.settings.OnOffContextMenu.Items;
import com.nonlinearlabs.client.world.maps.settings.OnOffContextMenu.Listener;

public class CrashOnError extends Setting {

	protected CrashOnError(DeveloperSettings parent) {
		super(parent, "Crash on Error", "Off");

		SetupModel.get().systemSettings.crashOnError.onChange(new Function<SetupModel.BooleanValues, Boolean>() {

			@Override
			public Boolean apply(SetupModel.BooleanValues t) {
				setCurrentValue(t == SetupModel.BooleanValues.on);
				return true;
			}
		});
	}

	@Override
	public IContextMenu createMenu(NonPosition pos) {
		NonLinearWorld world = getWorld();
		return world.setContextMenu(new OnOffContextMenu(new Listener() {

			@Override
			public void setValue(Items item) {
				SystemSettings.get().setCrashOnError(item == Items.ON ? BooleanValues.on : BooleanValues.off);
				EditBufferUseCases.get().setEnableUSBCommunication(item == Items.ON);
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
