package com.nonlinearlabs.client.presenters;

import java.util.HashMap;
import java.util.function.Function;

public class PresetPresenterProviders {
    private static PresetPresenterProviders theInstance = new PresetPresenterProviders();
    private HashMap<String, PresetPresenterProvider> consumers = new HashMap<String, PresetPresenterProvider>();

    public static PresetPresenterProviders get() {
        return theInstance;
    }

    public void register(String uuid, Function<PresetPresenter, Boolean> cb) {
        if (!consumers.containsKey(uuid))
            consumers.put(uuid, new PresetPresenterProvider(uuid));

        consumers.get(uuid).onChange(cb);
    }
}