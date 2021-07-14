package com.nonlinearlabs.client.world.maps.settings;

import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.maps.ContextMenu;
import com.nonlinearlabs.client.world.maps.NonPosition;
import com.nonlinearlabs.client.world.overlay.html.oleds.OledsDialog;

public class ShowOleds extends Setting {

    protected ShowOleds(DeveloperSettings parent) {
        super(parent, "Mirror Oleds", "Go!");
    }

    @Override
    public Control onValueButtonMouseDown(Position eventPoint) {
        OledsDialog.toggle();
        return this;
    }

    @Override
    public ContextMenu createMenu(NonPosition pos) {
        return null;
    }

    @Override
    public void setDefault() {
    }

}