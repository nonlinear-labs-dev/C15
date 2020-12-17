package com.nonlinearlabs.client.presenters;

import java.util.Date;

import com.google.gwt.i18n.client.DateTimeFormat;
import com.google.gwt.i18n.client.DateTimeFormat.PredefinedFormat;
import com.nonlinearlabs.client.GMTTimeZone;
import com.nonlinearlabs.client.dataModel.Notifier;
import com.nonlinearlabs.client.dataModel.presetManager.Bank;
import com.nonlinearlabs.client.dataModel.presetManager.Banks;
import com.nonlinearlabs.client.dataModel.presetManager.PresetManagerModel;

public class BankPresenterProvider extends Notifier<BankPresenter> {

    private BankPresenter thePresenter = new BankPresenter();
    private Bank model;

    public BankPresenterProvider(String uuid) {
        model = Banks.get().find(uuid);

        model.name.onChange(v -> updateName(model));
        model.orderNumber.onChange(v -> {
            thePresenter.orderNumber = v;
            updateName(model);
            return true;
        });

        model.uuid.onChange(v -> {
            thePresenter.uuid = v;
            notifyChanges();
            return true;
        });

        model.comment.onChange(v -> {
            if (thePresenter.comment != v) {
                thePresenter.comment = v;
                updateName(model);
            }
            return true;
        });

        model.presets.onChange(presets -> {
            thePresenter.presets = presets;
            updateSelectionOpportunities();
            notifyChanges();
            return true;
        });

        model.selectedPreset.onChange(v -> {
            updateSelectionOpportunities();
            return true;
        });

        model.dateOfExportFile.onChange(v -> mirrorString(thePresenter.dateOfExportFile, localizeTime(v)));
        model.dateOfImportFile.onChange(v -> mirrorString(thePresenter.dateOfImportFile, localizeTime(v)));
        model.dateOfLastChange.onChange(v -> mirrorString(thePresenter.dateOfLastChange, localizeTime(v)));

        model.nameOfExportFile.onChange(v -> mirrorString(thePresenter.nameOfExportFile, v));
        model.nameOfImportFile.onChange(v -> mirrorString(thePresenter.nameOfImportFile, v));
        model.importExportState.onChange(v -> mirrorString(thePresenter.importExportState, v));

        PresetManagerModel.get().selectedBank.onChange(p -> {
            var isSelected = uuid == p;
            if (thePresenter.isSelected != isSelected) {
                thePresenter.isSelected = isSelected;
                notifyChanges();
            }

            return true;
        });

        PresetManagerModel.get().midiBank.onChange(p -> {
            var isMidiBank = p == uuid;
            if (thePresenter.isMidiBank != isMidiBank) {
                thePresenter.isMidiBank = isMidiBank;
                updateName(model);
            }
            return true;
        });

    }

    private boolean updateName(Bank b) {
        thePresenter.name = b.orderNumber.getValue() + " - " + b.name.getValue();

        if (PresetManagerModel.get().midiBank.getValue() == model.uuid.getValue())
            thePresenter.name += " ^";

        notifyChanges();
        return true;
    }

    private boolean mirrorString(String target, String src) {
        if (target != src) {
            target = src;
            notifyChanges();
        }
        return true;
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

    private void updateSelectionOpportunities() {
        var idx = model.presets.getValue().indexOf(model.selectedPreset.getValue());
        var canPrevPreset = idx > 0;
        var canNextPreset = idx < model.presets.getValue().size() - 1;

        if (thePresenter.canSelectNextPreset != canNextPreset) {
            thePresenter.canSelectNextPreset = canNextPreset;
            notifyChanges();
        }

        if (thePresenter.canSelectPrevPreset != canPrevPreset) {
            thePresenter.canSelectPrevPreset = canPrevPreset;
            notifyChanges();
        }

    }

    @Override
    public BankPresenter getValue() {
        return thePresenter;
    }
}
