package com.nonlinearlabs.client.useCases;

import com.google.gwt.dom.client.DataTransfer.DropEffect;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.ServerProxy;
import com.nonlinearlabs.client.dataModel.presetManager.Bank;
import com.nonlinearlabs.client.dataModel.presetManager.Banks;
import com.nonlinearlabs.client.dataModel.presetManager.Preset;
import com.nonlinearlabs.client.dataModel.presetManager.Presets;

public class BankUseCases {
    private static BankUseCases theInstance = new BankUseCases();
    private static ServerProxy server = NonMaps.get().getServerProxy();

    public static BankUseCases get() {
        return theInstance;
    }

    public void selectPreset(String uuid) {
        Preset p = Presets.get().find(uuid);
        Bank b = Banks.get().find(p.bankUuid.getValue());
        b.selectedPreset.setValue(uuid);
        server.selectPreset(uuid);
    }

    public void move(String uuid, double x, double y) {
        Bank b = Banks.get().find(uuid);
        b.x.setValue(x);
        b.y.setValue(y);
        server.setBankPosition(uuid, x, y);
    }

    public void dropBelow(String presetUUID, String droppedType, String droppedData, DropEffect effect) {
        if (droppedType == "preset")
            if (effect == DropEffect.MOVE)
                server.movePresetBelow(droppedData, presetUUID);
            else if (effect == DropEffect.COPY)
                server.insertPresetCopyBelow(droppedData, presetUUID);
    }

    public void dropOn(String presetUUID, String droppedType, String droppedData, DropEffect effect) {
        if (droppedType == "preset")
            if (effect == DropEffect.MOVE)
                server.movePresetTo(droppedData, presetUUID);
            else if (effect == DropEffect.COPY)
                server.overwritePresetWith(droppedData, presetUUID);
    }

    public void dropAbove(String presetUUID, String droppedType, String droppedData, DropEffect effect) {
        if (droppedType == "preset")
            if (effect == DropEffect.MOVE)
                server.movePresetAbove(droppedData, presetUUID);
            else if (effect == DropEffect.COPY)
                server.insertPresetCopyAbove(droppedData, presetUUID);

    }

    public void dropOnBank(String bankUuid, String type, String data) {
        if (type == "preset") {
            var preset = Presets.get().find(data);
            if (preset.bankUuid.getValue() == bankUuid) {
                var bank = Banks.get().find(bankUuid);
                var presets = bank.presets.getValue();
                var lastPreset = presets.isEmpty() ? "" : presets.get(presets.size() - 1);
                server.movePresetBelow(data, lastPreset);
            } else {
                server.appendPreset(data, bankUuid);
            }
        } else if (type == "bank") {
            if (bankUuid != data) {
                server.dropBankOnBank(data, bankUuid);
            }
        }

    }

}
