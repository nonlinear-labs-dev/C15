package com.nonlinearlabs.client.dataModel.presetManager;

import java.util.ArrayList;

import com.nonlinearlabs.client.dataModel.BooleanDataModelEntity;
import com.nonlinearlabs.client.dataModel.DoubleDataModelEntity;
import com.nonlinearlabs.client.dataModel.IntegerDataModelEntity;
import com.nonlinearlabs.client.dataModel.StringDataModelEntity;

public class Bank extends Doomable {
	public PresetListDataModelEntity presets = new PresetListDataModelEntity(new ArrayList<String>());
	public StringDataModelEntity name = new StringDataModelEntity();
	public IntegerDataModelEntity orderNumber = new IntegerDataModelEntity();
	public StringDataModelEntity uuid = new StringDataModelEntity();
	public BooleanDataModelEntity isMidiBank = new BooleanDataModelEntity();
	public IntegerDataModelEntity orderNumber = new IntegerDataModelEntity();
	public StringDataModelEntity selectedPreset = new StringDataModelEntity();
	public DoubleDataModelEntity x = new DoubleDataModelEntity();
	public DoubleDataModelEntity y = new DoubleDataModelEntity();


	public PresetListDataModelEntity getPresets() {
		return presets;
	}

}
