package com.nonlinearlabs.client.dataModel.presetManager;

import com.nonlinearlabs.client.dataModel.DataModelEntity;
import com.nonlinearlabs.client.dataModel.IntegerDataModelEntity;
import com.nonlinearlabs.client.dataModel.StringDataModelEntity;

public class PresetManagerModel {
	static PresetManagerModel thePresetManager = new PresetManagerModel();

	public enum DragDataType {
		None, Preset, Bank, Presets
	}

	public static class DragDropData {
		public DragDropData() {
		}

		public DragDropData(DragDataType type, String data) {
			this.type = type;
			this.data = data;
		}

		public String data = "";
		public DragDataType type = DragDataType.None;
	}

	DragDropData dragDropData = new DragDropData();

	public BanksDataModelEntity banks = new BanksDataModelEntity();
	public BankPositionsDataModelEntity positions = new BankPositionsDataModelEntity();
	public StringDataModelEntity selectedBank = new StringDataModelEntity();
	public StringDataModelEntity midiBank = new StringDataModelEntity();
	public SelectedPresets selectedPresets = new SelectedPresets();
	public DataModelEntity<DragDropData> dnd = new DataModelEntity<DragDropData>(null);

	public DataModelEntity<CustomPresetSelection> customPresetSelection = new DataModelEntity<CustomPresetSelection>(
			null);

	public IntegerDataModelEntity fileVersion = new IntegerDataModelEntity();

	static public PresetManagerModel get() {
		return thePresetManager;
	}

}
