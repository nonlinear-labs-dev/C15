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
    private static PresetManagerModel model = PresetManagerModel.get();

    public static PresetManagerUseCases get() {
        return theInstance;
    }

    public void toggleMultipleSelection(String uuid) {
        var presets = model.selectedPresets.getValue();
        if (presets == null) {

            var preset = Presets.get().find(uuid);
            var bank = Banks.get().find(preset.bankUuid.getValue());
            var selPresetUuid = bank.selectedPreset.getValue();
            presets = new ArrayList<String>();
            presets.add(selPresetUuid);

            if (selPresetUuid != uuid)
                presets.add(uuid);

            model.selectedPresets.setValue(presets);
        } else {
            if (presets.contains(uuid) && presets.size() > 1)
                presets.remove(uuid);
            else
                presets.add(uuid);

            model.selectedPresets.notifyChanges();
        }
    }

    public void finishMultipleSelection() {
        model.selectedPresets.setValue(null);
    }

    public boolean hasMultipleSelection() {
        return model.selectedPresets.getValue() != null;
    }

    public String getSelectedPresetsCSV() {
        String csv = "";
        for (var a : model.selectedPresets.getValue()) {
            if (!csv.isEmpty())
                csv += ",";
            csv += a;
        }
        return csv;
    }

    public void resetDragDropData() {
        model.dnd.setValue(null);
    }

    public DragDropData setDragDropData(DragDataType type, String data) {
        model.dnd.setValue(new DragDropData(type, data));
        return model.dnd.getValue();
    }

    public void drop(double x, double y) {
        var dnd = model.dnd.getValue();
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
        PresetManagerUpdater u = new PresetManagerUpdater(null, model);
        u.updateBankPositions();
        server.setBankPosition(uuid, x, y, true);
    }

    public void selectBank(String uuid) {
        if (hasMultipleSelection()) {
            return;
        }

        model.selectedBank.setValue(uuid);
        server.selectBank(uuid);
    }

    public void moveBankBy(String uuid, String string) {
        server.moveBy(uuid, "LeftByOne");
    }

    public void deleteBank(String uuid) {
        server.deleteBank(uuid);
    }

    public void selectPreviousBank() {
        var sel = model.selectedBank.getValue();
        var banks = model.banks.getValue();
        var idx = banks.indexOf(sel);
        if (idx > 0) {
            var prevBank = banks.get(idx - 1);
            model.selectedBank.setValue(prevBank);
            server.selectBank(prevBank);
        }
    }

    public void selectNextBank() {
        var sel = model.selectedBank.getValue();
        var banks = model.banks.getValue();
        var idx = banks.indexOf(sel);
        if (idx + 1 < banks.size()) {
            var nextBank = banks.get(idx + 1);
            model.selectedBank.setValue(nextBank);
            server.selectBank(nextBank);
        }
    }
}