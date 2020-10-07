package com.nonlinearlabs.client.dataModel.presetManager;

import java.util.ArrayList;

import com.nonlinearlabs.client.dataModel.DataModelEntity;

public class SelectedPresets extends DataModelEntity<ArrayList<String>> {
    public SelectedPresets() {
        super(null);
    }

    @Override
    public void fromString(String str) {
    }
}