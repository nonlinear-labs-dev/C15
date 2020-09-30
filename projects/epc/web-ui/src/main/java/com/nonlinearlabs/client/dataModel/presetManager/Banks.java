package com.nonlinearlabs.client.dataModel.presetManager;

import java.util.HashMap;

public class Banks {
    private static Banks theInstance = new Banks();
    private HashMap<String, Bank> db = new HashMap<String, Bank>();

    public static Banks get() {
        return theInstance;
    }

    public Bank find(String uuid) {
        return db.get(uuid);
    }

    public Bank put(String uuid) {
        db.put(uuid, new Bank());
        return find(uuid);
    }

    public void preUpdate() {
        db.forEach((k, v) -> v.setDoomed());
    }

    public void postUpdate() {
        db.values().removeIf(v -> {
            if (v.isDoomed()) {
                Presets.get().onBankRemoved(v.uuid.getValue());
                return true;
            }
            return false;
        });
    }
}