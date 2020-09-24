package com.nonlinearlabs.client.useCases;

import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.dataModel.presetManager.Bank;
import com.nonlinearlabs.client.dataModel.presetManager.Banks;
import com.nonlinearlabs.client.dataModel.presetManager.Preset;
import com.nonlinearlabs.client.dataModel.presetManager.Presets;

public class BankUseCases {
    private static BankUseCases theInstance = new BankUseCases();

    public static BankUseCases get() {
        return theInstance;
    }

    public void selectPreset(String uuid) {
        Preset p = Presets.get().find(uuid);
        Bank b = Banks.get().find(p.bankUuid.getValue());
        b.selectedPreset.setValue(uuid);
        NonMaps.get().getServerProxy().selectPreset(uuid);
    }

    public void move(String uuid, double x, double y) {
        Bank b = Banks.get().find(uuid);
        b.x.setValue(x);
        b.y.setValue(y);
        NonMaps.get().getServerProxy().setBankPosition(uuid, x, y);
    }

    public void dropBelow(String presetUUID, String droppedType, String droppedData) {
        if (droppedType == "preset")
            NonMaps.get().getServerProxy().movePresetBelow(droppedData, presetUUID);
    }

    public void dropOn(String presetUUID, String droppedType, String droppedData) {
        if (droppedType == "preset")
            NonMaps.get().getServerProxy().overwritePresetWith(droppedData, presetUUID);
    }

    public void dropAbove(String presetUUID, String droppedType, String droppedData) {
        if (droppedType == "preset")
            NonMaps.get().getServerProxy().movePresetAbove(droppedData, presetUUID);
    }
}
