package com.nonlinearlabs.client.dataModel.presetManager;

import java.util.ArrayList;

import com.nonlinearlabs.client.dataModel.DataModelEntity;

public class BanksDataModelEntity extends DataModelEntity<ArrayList<String>> {

	public BanksDataModelEntity() {
		super(new ArrayList<String>());
	}

	@Override
	public void fromString(String str) {
		assert (false);
	}

}
