package com.nonlinearlabs.client.presenters;

import com.nonlinearlabs.client.dataModel.Notifier;
import com.nonlinearlabs.client.dataModel.presetManager.Bank;
import com.nonlinearlabs.client.dataModel.presetManager.Banks;

public class BankPresenterProvider extends Notifier<BankPresenter> {

    private BankPresenter thePresenter = new BankPresenter();

    public BankPresenterProvider(String uuid) {
        Bank b = Banks.get().find(uuid);

        b.name.onChange(v -> updateName(b));
        b.orderNumber.onChange(v -> updateName(b));

        b.presets.onChange(presets -> {
            thePresenter.presets = presets;
            notifyChanges();
            return true;
        });
    }

    private boolean updateName(Bank b) {
        thePresenter.name = b.orderNumber.getValue() + " - " + b.name.getValue();
        notifyChanges();
        return true;
    }

    @Override
    public BankPresenter getValue() {
        return thePresenter;
    }
}
