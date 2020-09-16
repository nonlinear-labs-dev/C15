package com.nonlinearlabs.client.dataModel.presetManager;

import java.util.ArrayList;

public class PresetManagerModel {

	static PresetManagerModel thePresetManager = new PresetManagerModel();

	private BankMapDataModelEntity banks = new BankMapDataModelEntity(new ArrayList<String>());

	static public PresetManagerModel get() {
		return thePresetManager;
	}

	public BankMapDataModelEntity getBanks() {
		return banks;
	}
}
