package com.nonlinearlabs.client.dataModel.presetManager;

import java.util.HashMap;

import com.nonlinearlabs.client.dataModel.DataModelEntity;

public class Presets extends DataModelEntity<HashMap<String, Preset>> {
    private static Presets theInstance = new Presets();

    public Presets() {
        super(new HashMap<String, Preset>());
    }

    public static Presets get() {
        return theInstance;
    }

    public Preset find(String uuid) {
        return getValue().get(uuid);
    }

    public Preset put(String uuid) {
        var v = getValue();
        v.put(uuid, new Preset());
        setValue(v);
        return find(uuid);
    }

    public void preUpdate(String bank) {
        getValue().forEach((k, v) -> {
            if (v.bankUuid.getValue() == bank)
                v.setDoomed();
        });
    }

    public void postUpdate() {
        var old = getValue();
        old.values().removeIf(v -> v.isDoomed());
        setValue(old);
    }

    public void onBankRemoved(String bankUuid) {
        var old = getValue();
        old.values().removeIf(v -> v.bankUuid.getValue() == bankUuid);
        setValue(old);
    }

    public boolean isEmpty() {
        return getValue().isEmpty();
    }
}