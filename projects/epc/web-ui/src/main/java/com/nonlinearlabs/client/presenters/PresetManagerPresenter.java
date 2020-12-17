package com.nonlinearlabs.client.presenters;

import java.util.ArrayList;

import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.VoiceGroup;
import com.nonlinearlabs.client.dataModel.presetManager.Bank.Position;

public class PresetManagerPresenter {

	public String[] loadModeMenuEntries = { "", "", "" };
	public ArrayList<String> banks = new ArrayList<String>();

	public boolean loadModeButtonState = false;

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

	public String loadedPresetNumber = "";

	public PresetManagerPresenter() {
	}

}
