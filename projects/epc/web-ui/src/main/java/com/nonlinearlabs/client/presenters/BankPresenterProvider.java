package com.nonlinearlabs.client.presenters;

import com.nonlinearlabs.client.dataModel.Notifier;
import com.nonlinearlabs.client.dataModel.presetManager.Bank;
import com.nonlinearlabs.client.dataModel.presetManager.Banks;

public class BankPresenterProvider extends Notifier<BankPresenter> {

    private BankPresenter thePresenter = new BankPresenter();

    public BankPresenterProvider(String uuid) {
        Bank b = Banks.get().find(uuid);

        b.name.onChange(v -> {
            thePresenter.name = v;
            notifyChanges();
            return true;
        });

        b.x.onChange(x -> {
            thePresenter.x = x;
            notifyChanges();
            return true;
        });

        b.y.onChange(y -> {
            thePresenter.y = y;
            notifyChanges();
            return true;
        });

        b.presets.onChange(presets -> {
            thePresenter.presets = presets;
            notifyChanges();
            return true;
        });
    }

    @Override
    public BankPresenter getValue() {
        return thePresenter;
    }

}
