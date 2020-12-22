package com.nonlinearlabs.client.world.overlay.belt.presets.html;

import com.google.gwt.dom.client.Style.Unit;
import com.google.gwt.user.client.ui.RootPanel;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.html.DropZone;

public class BeltPresetsUI extends DropZone {

    public BeltPresetsUI() {
        RootPanel.get().add(this);
        getElement().addClassName("belt-preset-ui");
    }

    public void syncPosition(Rect r) {
        getElement().getStyle().setLeft(r.getLeft(), Unit.PX);
        getElement().getStyle().setTop(r.getTop(), Unit.PX);
        getElement().getStyle().setWidth(r.getWidth(), Unit.PX);
        getElement().getStyle().setHeight(r.getHeight(), Unit.PX);
    }

}
