package com.nonlinearlabs.client.dataModel.clipboard;

import com.nonlinearlabs.client.dataModel.DataModelEntity;

public class ClipboardModel {
    static ClipboardModel theClipBoard = new ClipboardModel();

    static public ClipboardModel get() {
        return theClipBoard;
    }

    public enum DragDataType {
        None, Preset, Bank, Presets;

        public boolean any(DragDataType... a) {
            for (var p : a)
                if (this == p)
                    return true;

            return false;
        }
    }

    public static class DragDropData {
        public DragDropData() {
        }

        public DragDropData(DragDataType type, String data) {
            this.type = type;
            this.data = data;
        }

        public String data = "";
        public DragDataType type = DragDataType.None;
    }

    public DataModelEntity<DragDropData> dnd = new DataModelEntity<DragDropData>(null);
}