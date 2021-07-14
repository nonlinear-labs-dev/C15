package com.nonlinearlabs.client.dataModel;

public class HardwareSourceEnableSettingsDataModelEntity extends DataModelEntity<HardwareSourceEnableSettings> {

	public HardwareSourceEnableSettingsDataModelEntity() {
        super(new HardwareSourceEnableSettings(8, 4));
	}

	@Override
	public void fromString(String str) {
		setValue(HardwareSourceEnableSettings.fromString(str));
	}

}