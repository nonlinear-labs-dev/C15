package com.nonlinearlabs.client.dataModel;

import com.google.gwt.core.client.GWT;

public class RoutingAspectsDataModelEntity extends DataModelEntity<RoutingAspectsSetting> {

	public RoutingAspectsDataModelEntity() {
        super(new RoutingAspectsSetting());
	}

	@Override
	public void fromString(String str) {
		GWT.log(str);
		setValue(RoutingAspectsSetting.fromString(str));
	}

}