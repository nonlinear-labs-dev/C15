package com.nonlinearlabs.client.presenters;

import java.util.ArrayList;

import com.google.gwt.i18n.client.NumberFormat;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel;
import com.nonlinearlabs.client.world.maps.presets.bank.preset.Preset;

public class PresetManagerPresenter {
	public static class Bank {

		public static class Preset {
			public String uuid = "";
			public String name = "";
		}

		public Bank(com.nonlinearlabs.client.dataModel.presetManager.Bank bank) {
			bank.name.onChange(n -> {
				name = n;
				return true;
			});
		}

		public String name = "";
		public String uuid = "";
	}

	public ArrayList<Bank> banks;
	public String[] loadModeMenuEntries = { "", "", "" };
	public boolean loadModeButtonState = false;
	public boolean directLoadActive = false;
	public boolean loadToPartActive = false;

	public PresetManagerPresenter() {
		banks = new ArrayList<Bank>();
	}

	public String getBaseLoadedPresetNumberString() {
		String ret = "";
		Preset preset = NonMaps.get().getNonLinearWorld().getPresetManager().findLoadedPreset();
		com.nonlinearlabs.client.world.maps.presets.bank.Bank bank = preset != null ? preset.getParent() : null;

		if (EditBufferModel.get().loadedPreset.getValue().equals("Init")) {
			ret = "Init";
		} else if(EditBufferModel.get().loadedPreset.getValue().equals("Converted")) {
			ret = "Converted";
		} else if (bank == null && preset == null) {
			ret = "";
		} else if (bank != null && preset != null) {
			ret = bank.getOrderNumber() + "-" + NumberFormat.getFormat("000").format(preset.getNumber());
		}

		return ret;
	}

	public String getLoadedPresetNumberString() {
		boolean mod = EditBufferPresenterProvider.getPresenter().isAnyParameterChanged;
		String ret = getBaseLoadedPresetNumberString();

		if (ret.isEmpty())
			return ret;

		return ret += mod ? " *" : "";
	}

}
