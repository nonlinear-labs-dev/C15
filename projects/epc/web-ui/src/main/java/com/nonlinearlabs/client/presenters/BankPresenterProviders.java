package com.nonlinearlabs.client.presenters;

import java.util.HashMap;
import java.util.function.Function;

public class BankPresenterProviders {
    private static BankPresenterProviders theInstance = new BankPresenterProviders();
    private HashMap<String, BankPresenterProvider> consumers = new HashMap<String, BankPresenterProvider>();

    public static BankPresenterProviders get() {
        return theInstance;
    }

    public void register(String uuid, Function<BankPresenter, Boolean> cb) {
        if (!consumers.containsKey(uuid))
            consumers.put(uuid, new BankPresenterProvider(uuid));

        consumers.get(uuid).onChange(cb);
    }

    public BankPresenter getPresenter(String uuid) {
        if (!consumers.containsKey(uuid))
            consumers.put(uuid, new BankPresenterProvider(uuid));

        return consumers.get(uuid).getValue();
    }
}