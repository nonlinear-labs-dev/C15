package com.nonlinearlabs.client.dataModel.presetManager;

import java.util.ArrayList;

import com.nonlinearlabs.client.dataModel.DataModelEntity;
import com.nonlinearlabs.client.dataModel.presetManager.Bank.Position;

public class BankPositionsDataModelEntity extends DataModelEntity<ArrayList<Position>> {

    public BankPositionsDataModelEntity() {
        super(new ArrayList<Position>());
    }

    @Override
    public void fromString(String str) {
        assert (false);
    }

}
