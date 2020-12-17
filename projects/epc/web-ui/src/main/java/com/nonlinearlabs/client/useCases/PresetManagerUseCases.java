package com.nonlinearlabs.client.useCases;

import java.util.ArrayList;
import java.util.function.Function;

import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.ServerProxy;
import com.nonlinearlabs.client.ServerProxy.BankAndPosition;
import com.nonlinearlabs.client.dataModel.clipboard.ClipboardModel;
import com.nonlinearlabs.client.dataModel.clipboard.ClipboardModel.DragDataType;
import com.nonlinearlabs.client.dataModel.clipboard.ClipboardModel.DragDropData;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.VoiceGroup;
import com.nonlinearlabs.client.dataModel.presetManager.Bank;
import com.nonlinearlabs.client.dataModel.presetManager.Banks;
import com.nonlinearlabs.client.dataModel.presetManager.LoadToPartMode;
import com.nonlinearlabs.client.dataModel.presetManager.Preset.Color;
import com.nonlinearlabs.client.dataModel.presetManager.PresetManagerModel;
import com.nonlinearlabs.client.dataModel.presetManager.PresetManagerUpdater;
import com.nonlinearlabs.client.dataModel.presetManager.Presets;
import com.nonlinearlabs.client.dataModel.presetManager.StoreSelectMode;
import com.nonlinearlabs.client.dataModel.setup.SetupModel;
import com.nonlinearlabs.client.presenters.PresetManagerPresenterProvider;
import com.nonlinearlabs.client.world.maps.NonPosition;

public class PresetManagerUseCases {
    private static PresetManagerUseCases theInstance = new PresetManagerUseCases();
    private static ServerProxy server = NonMaps.get().getServerProxy();
    private static PresetManagerModel model = PresetManagerModel.get();
    private static ClipboardModel clipboardModel = ClipboardModel.get();

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
        clipboardModel.dnd.setValue(null);
    }

    public DragDropData setDragDropData(DragDataType type, String data) {
        clipboardModel.dnd.setValue(new DragDropData(type, data));
        return clipboardModel.dnd.getValue();
    }

    public void drop(double x, double y, Function<String, ArrayList<BankAndPosition>> getBankPositionsOfCluster) {
        var dnd = clipboardModel.dnd.getValue();
        if (dnd != null) {
            if (dnd.type == DragDataType.Bank)
                moveBank(dnd.data, x, y, getBankPositionsOfCluster);
            else if (dnd.type == DragDataType.Preset)
                server.createNewBankFromPreset(x, y, dnd.data);
            else if (dnd.type == DragDataType.Presets)
                server.createNewBankFromPresets(x, y, dnd.data);

            resetDragDropData();
        }
    }

    public void moveBank(String uuid, double x, double y,
            Function<String, ArrayList<BankAndPosition>> getBankPositionsOfCluster) {
        Bank b = Banks.get().find(uuid);
        b.x.setValue(x);
        b.y.setValue(y);
        b.attachedToBank.setValue("");
        b.attachDirection.setValue("");
        PresetManagerUpdater u = new PresetManagerUpdater(null, model);
        u.updateBankPositions();

        if (getBankPositionsOfCluster != null)
            server.setBankPositions(getBankPositionsOfCluster.apply(uuid));
    }

    public void selectBank(String uuid) {
        if (hasMultipleSelection())
            return;

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
        if (hasCustomPresetSelection()) {
            model.customPresetSelection.getValue().selectPreviousBank();
            return;
        }

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
        if (hasCustomPresetSelection()) {
            model.customPresetSelection.getValue().selectNextBank();
            return;
        }

        var sel = model.selectedBank.getValue();
        var banks = model.banks.getValue();
        var idx = banks.indexOf(sel);
        if (idx + 1 < banks.size()) {
            var nextBank = banks.get(idx + 1);
            model.selectedBank.setValue(nextBank);
            server.selectBank(nextBank);
        }
    }

    public void startStoreSelectMode() {
        if (isInLoadToPartMode())
            endLoadToPartMode();

        if (model.customPresetSelection.getValue() == null)
            if (!Presets.get().isEmpty())
                model.customPresetSelection.setValue(new StoreSelectMode());
    }

    public void endStoreSelectMode() {
        if (isInStoreSelectMode())
            model.customPresetSelection.setValue(null);
    }

    public void startLoadToPartMode() {
        if (isInStoreSelectMode())
            endStoreSelectMode();

        if (model.customPresetSelection.getValue() == null)
            if (!Presets.get().isEmpty())
                model.customPresetSelection.setValue(new LoadToPartMode());
    }

    public void endLoadToPartMode() {
        if (isInLoadToPartMode())
            model.customPresetSelection.setValue(null);
    }

    public boolean isInLoadToPartMode() {
        return model.customPresetSelection.getValue() instanceof LoadToPartMode;
    }

    public void loadSelectedPresetPart() {
        if (isInLoadToPartMode()) {
            LoadToPartMode selection = (LoadToPartMode) model.customPresetSelection.getValue();
            VoiceGroup currentVoiceGroup = EditBufferModel.get().voiceGroup.getValue();
            EditBufferUseCases.get().loadPresetPartIntoPart(selection.getSelectedPreset(), selection.getSelectedPart(),
                    currentVoiceGroup);
        }
    }

    public void selectPreviousPreset() {
        if (hasCustomPresetSelection()) {
            model.customPresetSelection.getValue().selectPreviousPreset();
            return;
        }

        var sel = model.selectedBank.getValue();
        var banks = Banks.get().find(sel);
        var preset = banks.selectedPreset.getValue();
        var presets = banks.presets.getValue();
        var idx = presets.indexOf(preset);

        if (idx > 0) {
            BankUseCases.get().selectPreset(banks.presets.getValue().get(idx - 1));
        }
    }

    public boolean hasCustomPresetSelection() {
        return model.customPresetSelection.getValue() != null;
    }

    public void selectNextPreset() {
        if (hasCustomPresetSelection()) {
            model.customPresetSelection.getValue().selectNextPreset();
            return;
        }

        var sel = model.selectedBank.getValue();
        var banks = Banks.get().find(sel);
        var preset = banks.selectedPreset.getValue();
        var presets = banks.presets.getValue();
        var idx = presets.indexOf(preset);

        if (idx < presets.size() - 1) {
            BankUseCases.get().selectPreset(banks.presets.getValue().get(idx + 1));
        }
    }

    public void loadSelectedPreset() {
        var sel = model.selectedBank.getValue();
        var banks = Banks.get().find(sel);
        var preset = banks.selectedPreset.getValue();
        server.loadPreset(preset);
    }

    public void createNewBank(NonPosition pos) {
        server.newBank("New Bank", pos);
    }

    private boolean isInStoreSelectMode() {
        return model.customPresetSelection.getValue() instanceof StoreSelectMode;
    }

    public void setPresetColor(String preset, Color color) {
        if (hasMultipleSelection()) {
            server.setPresetAttribute(getSelectedPresetsCSV(), "color", color.toString());
        } else {
            server.setPresetAttribute(preset, "color", color.toString());
        }
    }

    public void deletePresets(String csv, boolean withBank) {
        server.deletePresets(csv, withBank);
    }

    public void toggleStoreSelectMode() {
        if (isInStoreSelectMode()) {
            endStoreSelectMode();
        } else {
            startStoreSelectMode();
        }
    }

    public void storePreset() {
        boolean hasSelectedBank = !model.selectedBank.getValue().isEmpty();

        if (isInStoreSelectMode()) {
            var ssm = (StoreSelectMode) model.customPresetSelection.getValue();
            hasSelectedBank = ssm.getSelectedBank() != null;
        }

        if (!hasSelectedBank)
            createNewBank();
        else
            storeToBank();

        endStoreSelectMode();
    }

    public void storeToBank() {
        switch (SetupModel.get().systemSettings.presetStoreModeSetting.getValue()) {
            case append:
                if (isInStoreSelectMode()) {
                    var ssm = (StoreSelectMode) model.customPresetSelection.getValue();
                    server.appendEditBuffer(ssm.getSelectedBank());
                } else {
                    server.appendPreset();
                }
                break;

            case insert:
                if (isInStoreSelectMode()) {
                    var ssm = (StoreSelectMode) model.customPresetSelection.getValue();
                    server.insertPreset(ssm.getSelectedPreset());
                } else {
                    var p = PresetManagerPresenterProvider.get().getValue().selectedPreset;
                    server.insertPreset(p);
                }
                break;

            case overwrite:
                var selectedPreset = PresetManagerPresenterProvider.get().getValue().selectedPreset;
                if (isInStoreSelectMode()
                        && !((StoreSelectMode) model.customPresetSelection.getValue()).getSelectedPreset().isEmpty()) {
                    server.overwritePresetWithEditBuffer(
                            ((StoreSelectMode) model.customPresetSelection.getValue()).getSelectedPreset());
                } else if (!selectedPreset.isEmpty()) {
                    server.overwritePresetWithEditBuffer(selectedPreset);
                }
                break;
            default:
                break;
        }
    }

    protected void createNewBank() {
        server.newBankFromEditBuffer();
    }

}