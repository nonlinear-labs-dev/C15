package com.nonlinearlabs.client.dataModel.presetManager;

import java.util.ArrayList;

import com.nonlinearlabs.client.dataModel.DataModelEntity;

public class PresetListDataModelEntity extends DataModelEntity<ArrayList<String>> {

	public PresetListDataModelEntity(ArrayList<String> def) {
		super(def);
	}

	@Override
	public void fromString(String str) {
		assert (false);
	}

}
