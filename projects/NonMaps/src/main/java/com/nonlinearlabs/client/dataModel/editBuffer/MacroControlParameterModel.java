package com.nonlinearlabs.client.dataModel.editBuffer;

import com.google.gwt.xml.client.Node;
import com.nonlinearlabs.client.dataModel.StringDataModelEntity;
import com.nonlinearlabs.client.dataModel.Updater;

public class MacroControlParameterModel extends BasicParameterModel {
	public StringDataModelEntity givenName = new StringDataModelEntity();
	public StringDataModelEntity ogInfo = new StringDataModelEntity();
	public StringDataModelEntity ogGivenName = new StringDataModelEntity();

	public MacroControlParameterModel(ParameterId id) {
		super(id);

		givenName.onChange(e -> notifyChanges());
		ogInfo.onChange(e -> notifyChanges());
		ogGivenName.onChange(e -> notifyChanges());
	}

	public ParameterId[] getRouterIDs() {
		return ParameterFactory.getModulationRoutersForMC(id);
	}

	@Override
	public Updater createUpdater(Node c) {
		return new MacroControlParameterModelUpdater(c, this);
	}

}
