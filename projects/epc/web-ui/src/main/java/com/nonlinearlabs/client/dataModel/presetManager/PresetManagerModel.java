package com.nonlinearlabs.client.dataModel.presetManager;

public class PresetManagerModel {

	static PresetManagerModel thePresetManager = new PresetManagerModel();

	public BanksDataModelEntity banks = new BanksDataModelEntity();
	public BankPositionsDataModelEntity positions = new BankPositionsDataModelEntity();

	static public PresetManagerModel get() {
		return thePresetManager;
	}
}
