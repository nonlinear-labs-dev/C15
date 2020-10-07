package com.nonlinearlabs.client.presenters;

import java.util.Date;

import com.google.gwt.i18n.client.DateTimeFormat;
import com.google.gwt.i18n.client.DateTimeFormat.PredefinedFormat;
import com.google.gwt.i18n.client.NumberFormat;
import com.nonlinearlabs.client.GMTTimeZone;
import com.nonlinearlabs.client.dataModel.Notifier;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.SoundType;
import com.nonlinearlabs.client.dataModel.presetManager.Bank;
import com.nonlinearlabs.client.dataModel.presetManager.Banks;
import com.nonlinearlabs.client.dataModel.presetManager.Preset;
import com.nonlinearlabs.client.dataModel.presetManager.PresetManagerModel;
import com.nonlinearlabs.client.dataModel.presetManager.Presets;

public class PresetPresenterProvider extends Notifier<PresetPresenter> {

    private PresetPresenter thePresenter = new PresetPresenter();
    private Preset preset;

    public PresetPresenterProvider(String uuid) {
        preset = Presets.get().find(uuid);

        preset.name.onChange(v -> {
            thePresenter.name = v;
            notifyChanges();
            return true;
        });

        preset.number.onChange(v -> {
            thePresenter.rawNumber = v;
            thePresenter.paddedNumber = NumberFormat.getFormat("#000").format(v);
            notifyChanges();
            return true;
        });

        preset.type.onChange(t -> {
            if (thePresenter.type != t) {
                thePresenter.type = t;
                thePresenter.isDual = t != SoundType.Single;
                notifyChanges();
            }
            return true;
        });

        preset.deviceName.onChange(t -> {
            if (thePresenter.deviceName != t) {
                thePresenter.deviceName = t;
                notifyChanges();
            }
            return true;
        });

        preset.softwareVersion.onChange(t -> {
            if (thePresenter.softwareVersion != t) {
                thePresenter.softwareVersion = t;
                notifyChanges();
            }
            return true;
        });

        preset.storeTime.onChange(t -> {
            var localized = localizeTime(t);
            if (thePresenter.storeTime != localized) {
                thePresenter.storeTime = localized;
                notifyChanges();
            }
            return true;
        });

        preset.comment.onChange(t -> {
            if (thePresenter.comment != t) {
                thePresenter.comment = t;
                notifyChanges();
            }
            return true;
        });

        preset.storeCounter.onChange(t -> {
            if (thePresenter.storeCounter != t) {
                thePresenter.storeCounter = t;
                notifyChanges();
            }
            return true;
        });

        preset.color.onChange(t -> {
            if (thePresenter.color != t) {
                thePresenter.color = t;
                notifyChanges();
            }
            return true;
        });

        EditBufferModel.get().loadedPreset.onChange(loadedPresetUuid -> {
            updateLoaded();
            return true;
        });

        preset.bankUuid.onChange(bankUuid -> {
            thePresenter.bankUuid = bankUuid;

            Bank b = Banks.get().find(bankUuid);

            BankPresenterProviders.get().register(bankUuid, v -> {
                if (v.uuid != preset.bankUuid.getValue())
                    return false;

                thePresenter.bankName = v.name;
                return true;
            });

            if (b != null) {
                b.selectedPreset.onChange(selPreset -> {
                    if (bankUuid != preset.bankUuid.getValue()) {
                        return false;
                    }

                    updateSelected();
                    thePresenter.isLastPresetInBank = b.presets.getValue().size() == 1;
                    return true;
                });
            }
            return true;
        });

        PresetManagerModel.get().selectedPresets.onChange(s -> {
            updateLoaded();
            updateSelected();
            return true;
        });
    }

    private void updateSelected() {
        if (thePresenter.selected != isSelected()) {
            thePresenter.selected = !thePresenter.selected;
            notifyChanges();
        }
    }

    private boolean isSelected() {
        var currentSelection = PresetManagerModel.get().selectedPresets.getValue();
        if (currentSelection != null)
            return currentSelection.contains(preset.uuid.getValue());

        Bank b = Banks.get().find(preset.bankUuid.getValue());
        return b.selectedPreset.getValue() == preset.uuid.getValue();
    }

    private void updateLoaded() {
        if (thePresenter.loaded != isLoaded()) {
            thePresenter.loaded = !thePresenter.loaded;
            notifyChanges();
        }
    }

    private boolean isLoaded() {
        var currentSelection = PresetManagerModel.get().selectedPresets.getValue();
        if (currentSelection != null)
            return false; // no 'loaded' indication in multi-selection mode

        return EditBufferModel.get().loadedPreset.getValue() == preset.uuid.getValue();
    }

    @Override
    public PresetPresenter getValue() {
        return thePresenter;
    }

    private String localizeTime(String iso) {
        try {
            DateTimeFormat f = DateTimeFormat.getFormat("yyyy-MM-ddTHH:mm:ssZZZZ");
            Date d = f.parse(iso);
            DateTimeFormat locale = DateTimeFormat.getFormat(PredefinedFormat.DATE_TIME_SHORT);
            return locale.format(d, new GMTTimeZone());
        } catch (Exception e) {
            return iso;
        }
    }

}
