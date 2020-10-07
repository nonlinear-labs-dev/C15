package com.nonlinearlabs.client.presenters;

import java.util.ArrayList;

import com.google.gwt.i18n.client.NumberFormat;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel;
import com.nonlinearlabs.client.dataModel.setup.SetupModel;
import com.nonlinearlabs.client.dataModel.presetManager.Bank.Position;
import com.nonlinearlabs.client.dataModel.presetManager.PresetManagerModel;
import com.nonlinearlabs.client.world.maps.presets.bank.preset.Preset;

public class PresetManagerPresenter {

	public String[] loadModeMenuEntries = { "", "", "" };
	public ArrayList<String> banks;
	public boolean loadModeButtonState = false;
	public PresetManagerModel.DragDataType dndType = PresetManagerModel.DragDataType.None;
	public ArrayList<Position> bankPositions = new ArrayList<Position>();

	public String selectedBank = "";
	public String selectedPresetOfSelectedBank = "";

	public boolean multiSelection = false;
	public int numSelectedPresetsInMultiSelection = 0;
	public ArrayList<String> currentMultiSelection = new ArrayList<String>();

	public PresetManagerPresenter() {
		banks = new ArrayList<Bank>();
	}

	public String getBaseLoadedPresetNumberString() {
		String ret = "";
		Preset preset = NonMaps.get().getNonLinearWorld().getPresetManager().findLoadedPreset();
		com.nonlinearlabs.client.world.maps.presets.bank.Bank bank = preset != null ? preset.getParent() : null;

		if (EditBufferModel.get().loadedPreset.getValue().equals("Init")) {
			ret = "Init";
		} else if (EditBufferModel.get().loadedPreset.getValue().equals("Converted")) {
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

		return ret + (mod ? " *" : "");
	}
}
