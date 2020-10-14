package com.nonlinearlabs.client.useCases;

import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.ServerProxy;

public class ClipboardUseCases {
    private static ClipboardUseCases theInstance = new ClipboardUseCases();
    private static ServerProxy server = NonMaps.get().getServerProxy();

    public static ClipboardUseCases get() {
        return theInstance;
    }

    public void cutPreset(String preset) {
        server.cutPreset(preset);
    }

    public void copyPreset(String preset) {
        server.copyPreset(preset);
    }

    public void pasteOnPreset(String preset) {
        server.pasteOnPreset(preset);
    }

    public void copyPresets() {
        if (PresetManagerUseCases.get().hasMultipleSelection())
            server.copyPresets(PresetManagerUseCases.get().getSelectedPresetsCSV());
    }

    public void copyBank(String uuid) {
        server.copyBank(uuid);
    }

    public void pasteOnBank(String uuid) {
        server.pasteOnBank(uuid);
    }
}