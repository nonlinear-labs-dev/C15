package com.nonlinearlabs.client.dataModel.presetManager;

import com.nonlinearlabs.client.dataModel.IntegerDataModelEntity;
import com.nonlinearlabs.client.dataModel.StringDataModelEntity;

public class Preset extends Doomable {
	public StringDataModelEntity name = new StringDataModelEntity();
	public StringDataModelEntity uuid = new StringDataModelEntity();
	public StringDataModelEntity bankUuid = new StringDataModelEntity();
	public IntegerDataModelEntity number = new IntegerDataModelEntity();
}