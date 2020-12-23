package com.nonlinearlabs.client.world.overlay.belt.presets.html;

import com.google.gwt.dom.client.Document;
import com.google.gwt.event.dom.client.DragEndEvent;
import com.google.gwt.user.client.ui.HTMLPanel;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.useCases.PresetManagerUseCases;
import com.nonlinearlabs.client.world.PointerEvent;

class BankUI extends HTMLPanel {

    class BankContent extends HTMLPanel {
        public BankContent() {
            super("");
            getElement().addClassName("content");
            add(new BankHeaderUI());
            add(new PresetListUI());
        }
    }

    public BankUI() {
        super("");

        getElement().addClassName("bank");
        add(new BankContent());

        addDomHandler(e -> {
            e.stopPropagation();
            Document.get().getElementById("drag-drop-image").removeFromParent();
            PresetManagerUseCases.get().resetDragDropData();
        }, DragEndEvent.getType());

        PointerEvent pointerDown = new PointerEvent("pointerdown");

        addDomHandler(c -> {
            NonMaps.get().getNonLinearWorld().getViewport().getOverlay().removeExistingContextMenus();
        }, pointerDown.eventType);
    }

}