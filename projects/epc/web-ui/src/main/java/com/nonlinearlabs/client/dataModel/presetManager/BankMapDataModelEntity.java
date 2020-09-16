package com.nonlinearlabs.client.dataModel.presetManager;

import java.util.ArrayList;

import com.nonlinearlabs.client.dataModel.DataModelEntity;

public class BankMapDataModelEntity extends DataModelEntity<ArrayList<String>> {

	public BankMapDataModelEntity(ArrayList<String> def) {
		super(def);
	}

	@Override
	public void fromString(String str) {
		assert (false);
	}

}
