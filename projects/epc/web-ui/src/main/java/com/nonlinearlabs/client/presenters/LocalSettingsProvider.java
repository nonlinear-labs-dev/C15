package com.nonlinearlabs.client.presenters;

import com.nonlinearlabs.client.dataModel.Notifier;
import com.nonlinearlabs.client.dataModel.setup.SetupModel;
import com.nonlinearlabs.client.dataModel.setup.SetupModel.BooleanValues;

public class LocalSettingsProvider extends Notifier<LocalSettings> {
	public static LocalSettingsProvider theInstance = new LocalSettingsProvider();

	public static LocalSettingsProvider get() {
		return theInstance;
	}

	private LocalSettings settings = new LocalSettings();

	private LocalSettingsProvider() {
		com.nonlinearlabs.client.dataModel.setup.SetupModel.LocalSettings s = SetupModel.get().localSettings;

		s.selectionAutoScroll.onChange(t -> {
			settings.selectionAutoScroll.selected = t.ordinal();
			settings.selectionAutoScroll.selectedOption = t;
			notifyChanges();
			return true;
		});

		s.editParameter.onChange(t -> {
			settings.editParameter.selected = t.ordinal();
			notifyChanges();
			return true;
		});

		s.displayScaling.onChange(t -> {
			settings.displayScalingFactor.selected = t.ordinal();
			notifyChanges();
			return true;
		});

		s.stripeBrightness.onChange(t -> {
			settings.stripeBrightness.selected = t.ordinal();
			notifyChanges();
			return true;
		});

		s.contextMenus.onChange(t -> {
			settings.contextMenus.value = t == BooleanValues.on;
			notifyChanges();
			return true;
		});

		s.presetDragDrop.onChange(t -> {
			settings.presetDragDrop.value = t == BooleanValues.on;
			notifyChanges();
			return true;
		});

		s.bitmapCache.onChange(t -> {
			settings.bitmapCache.value = t == BooleanValues.on;
			notifyChanges();
			return true;
		});

		s.showDeveloperOptions.onChange(t -> {
			settings.showDeveloperOptions.value = t == BooleanValues.on;
			notifyChanges();
			return true;
		});
	}

	@Override
	public LocalSettings getValue() {
		return settings;
	}
}
