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
        notifyChanges(false);
        return find(uuid);
    }

    public void preUpdate(String bank) {
        getValue().forEach((k, v) -> {
            if (v.bankUuid.getValue() == bank)
                v.setDoomed();
        });
    }

    public void postUpdate() {
        var v = getValue();
        v.values().removeIf(a -> a.isDoomed());
        notifyChanges(false);
    }

    public void onBankRemoved(String bankUuid) {
        var v = getValue();
        v.values().removeIf(a -> a.bankUuid.getValue() == bankUuid);
        notifyChanges(false);
    }

    public boolean isEmpty() {
        return getValue().isEmpty();
    }
}