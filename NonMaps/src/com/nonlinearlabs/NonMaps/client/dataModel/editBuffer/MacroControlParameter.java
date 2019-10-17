package com.nonlinearlabs.NonMaps.client.dataModel.editBuffer;

import com.nonlinearlabs.NonMaps.client.dataModel.StringDataModelEntity;

public class MacroControlParameter extends BasicParameterModel {
	public MacroControlParameter(int id) {
		super(id);
	}

	public StringDataModelEntity givenName = new StringDataModelEntity();
	public StringDataModelEntity info = new StringDataModelEntity();
}
