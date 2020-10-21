package com.nonlinearlabs.client.useCases;

import com.google.gwt.dom.client.DataTransfer.DropEffect;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.ServerProxy;
import com.nonlinearlabs.client.dataModel.presetManager.Bank;
import com.nonlinearlabs.client.dataModel.presetManager.Banks;
import com.nonlinearlabs.client.dataModel.presetManager.Preset;
import com.nonlinearlabs.client.dataModel.presetManager.PresetManagerModel;
import com.nonlinearlabs.client.dataModel.presetManager.PresetManagerModel.DragDataType;
import com.nonlinearlabs.client.dataModel.presetManager.Presets;
import com.nonlinearlabs.client.world.maps.NonPosition;

public class BankUseCases {

    private static BankUseCases theInstance = new BankUseCases();
    private static ServerProxy server = NonMaps.get().getServerProxy();

    public static BankUseCases get() {
        return theInstance;
    }

    public void selectPreset(String uuid) {
        PresetManagerUseCases.get().finishMultipleSelection();
        Preset p = Presets.get().find(uuid);
        Bank b = Banks.get().find(p.bankUuid.getValue());
        b.selectedPreset.setValue(uuid);
        PresetManagerModel.get().selectedBank.setValue(b.uuid.getValue());
        server.selectPreset(uuid);
    }

    public void dropBelow(String presetUUID, DropEffect effect) {
        var dnd = PresetManagerModel.get().dnd.getValue();
        if (dnd == null)
            return;

        if (dnd.type == DragDataType.Preset) {
            if (effect == DropEffect.MOVE)
                server.movePresetBelow(dnd.data, presetUUID);
            else if (effect == DropEffect.COPY)
                server.insertPresetCopyBelow(dnd.data, presetUUID);
        } else if (dnd.type == DragDataType.Presets) {
            server.dropPresetsBelow(dnd.data, presetUUID);
        } else if (dnd.type == DragDataType.Bank) {
            server.insertBankBelow(dnd.data, presetUUID);
        }
    }

    public void dropOn(String presetUUID, DropEffect effect) {
        var dnd = PresetManagerModel.get().dnd.getValue();
        if (dnd == null)
            return;

        if (dnd.type == DragDataType.Preset) {
            if (effect == DropEffect.MOVE)
                server.movePresetTo(dnd.data, presetUUID);
            else if (effect == DropEffect.COPY)
                server.overwritePresetWith(dnd.data, presetUUID);
        } else if (dnd.type == DragDataType.Presets) {
            server.dropPresetsTo(dnd.data, presetUUID);
        } else if (dnd.type == DragDataType.Bank) {
            server.overwritePresetWithBank(dnd.data, presetUUID);
        }
    }

    public void dropAbove(String presetUUID, DropEffect effect) {
        var dnd = PresetManagerModel.get().dnd.getValue();
        if (dnd == null)
            return;

        if (dnd.type == DragDataType.Preset) {
            if (effect == DropEffect.MOVE)
                server.movePresetAbove(dnd.data, presetUUID);
            else if (effect == DropEffect.COPY)
                server.insertPresetCopyAbove(dnd.data, presetUUID);
        } else if (dnd.type == DragDataType.Presets) {
            server.dropPresetsAbove(dnd.data, presetUUID);
        } else if (dnd.type == DragDataType.Bank) {
            server.insertBankAbove(dnd.data, presetUUID);
        }
    }

    public void dropOnBank(String bankUuid) {
        var dnd = PresetManagerModel.get().dnd.getValue();
        if (dnd == null)
            return;

        if (dnd.type == DragDataType.Preset) {
            var preset = Presets.get().find(dnd.data);
            if (preset.bankUuid.getValue() == bankUuid) {
                var bank = Banks.get().find(bankUuid);
                var presets = bank.presets.getValue();
                var lastPreset = presets.isEmpty() ? "" : presets.get(presets.size() - 1);
                server.movePresetBelow(dnd.data, lastPreset);
            } else {
                server.appendPreset(dnd.data, bankUuid);
            }
        } else if (dnd.type == DragDataType.Bank) {
            if (bankUuid != dnd.data) {
                server.dropBankOnBank(dnd.data, bankUuid);
            }
        } else if (dnd.type == DragDataType.Presets) {
            server.dropPresetsOnBank(dnd.data, bankUuid);
        }
    }

    public enum TapePosition {
        West, East, North, South
    }

    public void dock(String client, String master, TapePosition tapePosition, NonPosition pos) {
        server.dockBanks(master, tapePosition, client, pos);
    }

    public void dropDock(String master, TapePosition tapePosition, NonPosition pos) {
        var dnd = PresetManagerModel.get().dnd.getValue();
        if (dnd != null && dnd.type == DragDataType.Bank)
            dock(dnd.data, master, tapePosition, pos);
    }

    public void rename(String uuid, String newName) {
        Banks.get().find(uuid).name.setValue(newName);
        server.renameBank(uuid, newName);
    }

    public void setComment(String uuid, String text) {
        Banks.get().find(uuid).comment.setValue(text);
        server.setBankAttribute(uuid, "Comment", text);
    }

    public void setOrderNumber(String uuid, int v) {
        Banks.get().find(uuid).orderNumber.setValue(v);
        server.setBankOrderNumber(uuid, v);
    }

}
