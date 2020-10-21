package com.nonlinearlabs.client.dataModel.presetManager;

import java.util.ArrayList;
import java.util.HashMap;

import com.nonlinearlabs.client.dataModel.presetManager.Bank.Position;

public class Banks {
    private static Banks theInstance = new Banks();
    private HashMap<String, Bank> db = new HashMap<String, Bank>();

    public static Banks get() {
        return theInstance;
    }

    public Bank find(String uuid) {
        return db.get(uuid);
    }

    public Bank findOrPut(String v) {
        var b = find(v);
        if (b != null)
            return b;
        return put(v);
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

    public ArrayList<Position> updatePositions() {
        var positions = new ArrayList<Position>();

        for (var b : db.values()) {
            var uuid = b.attachedToBank.getValue();
            var dir = b.attachDirection.getValue();

            if (uuid.isEmpty() || find(uuid) == null) {
                positions.add(new Bank.AbsolutePosition(b.uuid.getValue(), b.x.getValue(), b.y.getValue()));
            } else {
                positions.add(new Bank.RelativePosition(b.uuid.getValue(), uuid, dir, b.x.getValue(), b.y.getValue()));
            }
        }

        positions.sort((a, b) -> a.bank.compareTo(b.bank));
        return positions;
    }

    public Bank findByName(String bankName) {
        for (var b : db.values())
            if (b.name.getValue() == bankName)
                return b;
        return null;
    }

}