package com.nonlinearlabs.client.presenters;

import com.google.gwt.i18n.client.NumberFormat;
import com.nonlinearlabs.client.dataModel.Notifier;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel;
import com.nonlinearlabs.client.dataModel.presetManager.Bank;
import com.nonlinearlabs.client.dataModel.presetManager.Banks;
import com.nonlinearlabs.client.dataModel.presetManager.Preset;
import com.nonlinearlabs.client.dataModel.presetManager.Presets;

public class PresetPresenterProvider extends Notifier<PresetPresenter> {

    private PresetPresenter thePresenter = new PresetPresenter();

    public PresetPresenterProvider(String uuid) {
        Preset p = Presets.get().find(uuid);

        p.name.onChange(v -> {
            thePresenter.name = v;
            notifyChanges();
            return true;
        });

        p.number.onChange(v -> {
            thePresenter.number = NumberFormat.getFormat("#000").format(v);
            return true;
        });

        EditBufferModel.get().loadedPreset.onChange(loadedPresetUuid -> {
            boolean isLoaded = uuid == loadedPresetUuid;
            if (thePresenter.loaded != isLoaded) {
                thePresenter.loaded = isLoaded;
                notifyChanges();
            }
            return true;
        });

        p.bankUuid.onChange(bankUuid -> {
            Bank b = Banks.get().find(bankUuid);

            if (b == null)
                return true;

            b.selectedPreset.onChange(selPreset -> {
                if (bankUuid != p.bankUuid.getValue())
                    return false;

                boolean isSelected = selPreset == uuid;
                if (thePresenter.selected != isSelected) {
                    thePresenter.selected = isSelected;
                    notifyChanges();
                }
                return true;
            });

            return true;
        });

    }

    @Override
    public PresetPresenter getValue() {
        return thePresenter;
    }

}
