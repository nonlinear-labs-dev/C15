package com.nonlinearlabs.client.presenters;

import java.util.ArrayList;

import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel;
import com.nonlinearlabs.client.dataModel.setup.SetupModel;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.VoiceGroup;
import com.nonlinearlabs.client.dataModel.presetManager.Bank.Position;
import com.nonlinearlabs.client.dataModel.presetManager.Banks;
import com.nonlinearlabs.client.dataModel.presetManager.PresetManagerModel;
import com.nonlinearlabs.client.dataModel.presetManager.Presets;

public class PresetManagerPresenter {

	public String[] loadModeMenuEntries = { "", "", "" };
	public ArrayList<String> banks = new ArrayList<String>();

	public boolean loadModeButtonState = false;
	public PresetManagerModel.DragDataType dndType = PresetManagerModel.DragDataType.None;
	public ArrayList<Position> bankPositions = new ArrayList<Position>();

	// TODO: check that selected preset is bound to load to part and other selection
	// modes
	public String selectedBank = "";
	public String selectedPreset = "";

	// TODO set selected part
	public VoiceGroup selectedPart = VoiceGroup.Global;

	public boolean multiSelection = false;
	public int numSelectedPresetsInMultiSelection = 0;
	public ArrayList<String> currentMultiSelection = new ArrayList<String>();

	public boolean inLoadToPartMode = false;
	public boolean inStoreSelectMode = false;

	public int fileVersion = 0;
	public boolean hasPresets = false;

	public boolean canSelectNextBank = false;
	public boolean canSelectPrevBank = false;
	public boolean canSelectNextPreset = false;
	public boolean canSelectPrevPreset = false;

	public PresetManagerPresenter() {
	}

	public String getBaseLoadedPresetNumberString() {
		String ret = "";

		if (EditBufferModel.get().loadedPreset.getValue().equals("Init")) {
			ret = "Init";
		} else if (EditBufferModel.get().loadedPreset.getValue().equals("Converted")) {
			ret = "Converted";
		} else if (bank == null && preset == null) {
			ret = "";
		} else if (bank != null && preset != null) {
			ret = bank.getOrderNumber() + "-" + NumberFormat.getFormat("000").format(preset.getNumber());
		} else {
			var loadedPresetUuid = EditBufferModel.get().loadedPreset.getValue();
			var loadedPreset = Presets.get().find(loadedPresetUuid);
			var bankUuid = loadedPreset != null ? loadedPreset.bankUuid.getValue() : "";
			var bank = Banks.get().find(bankUuid);

			if (bank != null && loadedPreset != null) {
				var bankOrderNumber = BankPresenterProviders.get().getPresenter(bankUuid).orderNumber;
				var presetOrderNumber = PresetPresenterProviders.get().getPresenter(loadedPresetUuid).paddedNumber;
				ret = bankOrderNumber + "-" + presetOrderNumber;
			}
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
