package com.nonlinearlabs.client.useCases;

import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.ServerProxy;
import com.nonlinearlabs.client.dataModel.clipboard.ClipboardModel;
import com.nonlinearlabs.client.dataModel.clipboard.ClipboardModel.DragDataType;
import com.nonlinearlabs.client.dataModel.clipboard.ClipboardModel.DragDropData;

public class PresetUseCases {
    private static PresetUseCases theInstance = new PresetUseCases();
    private static ServerProxy server = NonMaps.get().getServerProxy();

    public static PresetUseCases get() {
        return theInstance;
    }

    public DragDropData startDrag(String presetUuid) {
        if (PresetManagerUseCases.get().hasMultipleSelection()) {
            String csv = PresetManagerUseCases.get().getSelectedPresetsCSV();
            PresetManagerUseCases.get().setDragDropData(DragDataType.Presets, csv);

        } else {
            PresetManagerUseCases.get().setDragDropData(DragDataType.Preset, presetUuid);
        }
        return ClipboardModel.get().dnd.getValue();

    }

    public void rename(String uuid, String newName) {
        server.renamePreset(uuid, newName);
    }

}