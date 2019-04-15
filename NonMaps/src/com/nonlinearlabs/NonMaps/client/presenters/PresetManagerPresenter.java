package com.nonlinearlabs.NonMaps.client.presenters;

import java.util.ArrayList;

import com.google.gwt.i18n.client.NumberFormat;
import com.nonlinearlabs.NonMaps.client.NonMaps;
import com.nonlinearlabs.NonMaps.client.dataModel.editBuffer.EditBufferModel;
import com.nonlinearlabs.NonMaps.client.world.maps.presets.bank.preset.Preset;

public class PresetManagerPresenter {
	public static class Bank {

		public static class Preset {
			public String uuid;
			public String name;
		}

		public Bank(com.nonlinearlabs.NonMaps.client.dataModel.presetManager.Bank bank) {
			bank.name.onChange(n -> {
				name = n;
				return true;
			});
		}

		public String name;
		public String uuid;
	}

	public ArrayList<Bank> banks;

	public PresetManagerPresenter() {
		banks = new ArrayList<Bank>();
	}
	
	public String getBaseLoadedPresetNumberString() {		
		EditBufferModel eb = EditBufferModel.get();
		String ret = "";
		Preset preset = NonMaps.get().getNonLinearWorld().getPresetManager().findLoadedPreset();
		com.nonlinearlabs.NonMaps.client.world.maps.presets.bank.Bank bank = preset != null ? preset.getParent() : null;
		
		if(eb.loadedPreset.getValue().equals("Init")) {
			ret = "Init";
		} else if(bank == null && preset == null) {
			ret = "";
		} else if(bank != null && preset != null) {
			ret = bank.getOrderNumber() + "-" + NumberFormat.getFormat("000").format(preset.getNumber()); 
		}

		return ret;
	}
	
	public String getLoadedPresetNumberString() {
		boolean mod = EditBufferModel.get().isAnyParamChanged();
		String ret = getBaseLoadedPresetNumberString();
		
		if(ret.isEmpty())
			return ret;
		
		return ret += mod ? " *" : "";
	}
	
}
