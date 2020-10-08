package com.nonlinearlabs.client.useCases;

import java.util.ArrayList;

import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.ServerProxy;
import com.nonlinearlabs.client.dataModel.presetManager.Bank;
import com.nonlinearlabs.client.dataModel.presetManager.Banks;
import com.nonlinearlabs.client.dataModel.presetManager.PresetManagerModel;
import com.nonlinearlabs.client.dataModel.presetManager.PresetManagerModel.DragDataType;
import com.nonlinearlabs.client.dataModel.presetManager.PresetManagerModel.DragDropData;
import com.nonlinearlabs.client.dataModel.presetManager.PresetManagerUpdater;
import com.nonlinearlabs.client.dataModel.presetManager.Presets;

public class PresetManagerUseCases {
    private static PresetManagerUseCases theInstance = new PresetManagerUseCases();
    private static ServerProxy server = NonMaps.get().getServerProxy();

    public static PresetManagerUseCases get() {
        return theInstance;
    }

    public void toggleMultipleSelection(String uuid) {
        var presets = PresetManagerModel.get().selectedPresets.getValue();
        if (presets == null) {

            var preset = Presets.get().find(uuid);
            var bank = Banks.get().find(preset.bankUuid.getValue());
            var selPresetUuid = bank.selectedPreset.getValue();
            presets = new ArrayList<String>();
            presets.add(selPresetUuid);

            if (selPresetUuid != uuid)
                presets.add(uuid);

            PresetManagerModel.get().selectedPresets.setValue(presets);
        } else {
            if (presets.contains(uuid) && presets.size() > 1)
                presets.remove(uuid);
            else
                presets.add(uuid);

            PresetManagerModel.get().selectedPresets.notifyChanges();
        }
    }

    public void finishMultipleSelection() {
        PresetManagerModel.get().selectedPresets.setValue(null);
    }

    public boolean hasMultipleSelection() {
        return PresetManagerModel.get().selectedPresets.getValue() != null;
    }

    public String getSelectedPresetsCSV() {
        String csv = "";
        for (var a : PresetManagerModel.get().selectedPresets.getValue()) {
            if (!csv.isEmpty())
                csv += ",";
            csv += a;
        }
        return csv;
    }

    public void resetDragDropData() {
        PresetManagerModel.get().dnd.setValue(null);
    }

    public DragDropData setDragDropData(DragDataType type, String data) {
        PresetManagerModel.get().dnd.setValue(new DragDropData(type, data));
        return PresetManagerModel.get().dnd.getValue();
    }

    public void drop(double x, double y) {
        var dnd = PresetManagerModel.get().dnd.getValue();
        if (dnd != null) {
            if (dnd.type == DragDataType.Bank)
                moveBank(dnd.data, x, y);
            else if (dnd.type == DragDataType.Preset)
                server.createNewBankFromPreset(x, y, dnd.data);
            else if (dnd.type == DragDataType.Presets)
                server.createNewBankFromPresets(x, y, dnd.data);

            resetDragDropData();
        }
    }

    public void moveBank(String uuid, double x, double y) {
        Bank b = Banks.get().find(uuid);
        b.x.setValue(x);
        b.y.setValue(y);
        b.attachedToBank.setValue("");
        b.attachDirection.setValue("");
        PresetManagerUpdater u = new PresetManagerUpdater(null, PresetManagerModel.get());
        u.updateBankPositions();
        server.setBankPosition(uuid, x, y, true);
    }

    public void selectBank(String uuid) {
        if (hasMultipleSelection()) {
            return;
        }

        PresetManagerModel.get().selectedBank.setValue(uuid);
        server.selectBank(uuid);
    }
}