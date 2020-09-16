package com.nonlinearlabs.client.dataModel.presetManager;

import java.util.HashMap;

public class Presets {
    private static Presets theInstance = new Presets();
    private HashMap<String, Preset> db = new HashMap<String, Preset>();

    public static Presets get() {
        return theInstance;
    }

    public Preset find(String uuid) {
        return db.get(uuid);
    }

    public Preset put(String uuid) {
        db.put(uuid, new Preset());
        return find(uuid);
    }

    public void preUpdate(String bank) {
        db.forEach((k, v) -> {
            if (v.bankUuid.getValue() == bank)
                v.setDoomed();
        });
    }

    public void postUpdate(String bankUuid) {
        db.values().removeIf(v -> v.isDoomed() && v.bankUuid.getValue() == bankUuid);
    }

    public void onBankRemoved(String bankUuid) {
        db.values().removeIf(v -> v.bankUuid.getValue() == bankUuid);
    }
}