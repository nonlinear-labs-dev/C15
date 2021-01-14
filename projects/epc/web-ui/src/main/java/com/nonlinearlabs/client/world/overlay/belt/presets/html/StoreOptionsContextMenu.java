package com.nonlinearlabs.client.world.overlay.belt.presets.html;

import com.nonlinearlabs.client.dataModel.setup.SetupModel.PresetStoreMode;
import com.nonlinearlabs.client.useCases.SystemSettings;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.overlay.ContextMenu;
import com.nonlinearlabs.client.world.overlay.ContextMenuItem;
import com.nonlinearlabs.client.world.overlay.OverlayLayout;

class StoreOptionsContextMenu extends ContextMenu {

    /**
     *
     */
    private final BeltPresets beltPresets;

    public StoreOptionsContextMenu(BeltPresets beltPresets, OverlayLayout parent) {
        super(parent);
        this.beltPresets = beltPresets;

        if (this.beltPresets.deviceSettings.presetStoreMode != PresetStoreMode.append) {
            addChild(new ContextMenuItem(this, "Append") {
                @Override
                public Control click(Position eventPoint) {
                    SystemSettings.get().setPresetStoreMode(PresetStoreMode.append);
                    return super.click(eventPoint);
                }
            });
        }

        if (this.beltPresets.deviceSettings.presetStoreMode != PresetStoreMode.insert) {
            addChild(new ContextMenuItem(this, "Insert") {
                @Override
                public Control click(Position eventPoint) {
                    SystemSettings.get().setPresetStoreMode(PresetStoreMode.insert);
                    return super.click(eventPoint);
                }
            });
        }
        if (this.beltPresets.deviceSettings.presetStoreMode != PresetStoreMode.overwrite) {
            addChild(new ContextMenuItem(this, "Overwrite") {
                @Override
                public Control click(Position eventPoint) {
                    SystemSettings.get().setPresetStoreMode(PresetStoreMode.overwrite);
                    return super.click(eventPoint);
                }
            });
        }
    }
}