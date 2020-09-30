package com.nonlinearlabs.client.world.maps.presets.html;

import com.google.gwt.event.dom.client.DropEvent;
import com.google.gwt.user.client.ui.Label;
import com.nonlinearlabs.client.useCases.BankUseCases;
import com.nonlinearlabs.client.world.maps.presets.html.PresetManagerUI.DragDropData;

class BankHeaderUI extends DropZone {
    Label text;

    BankHeaderUI(String bankUuid) {
        getElement().addClassName("header");
        add(text = new Label());

        addDomHandler(e -> {
            getElement().removeClassName("drop-target");
            e.preventDefault();
            e.stopPropagation();
            DragDropData dnd = PresetManagerUI.get().getDragDropData();
            BankUseCases.get().dropOnBank(bankUuid, dnd.type, dnd.data);
            e.getDataTransfer().clearData();
        }, DropEvent.getType());
    }

    public void setText(String text) {
        this.text.setText(text);
    }
}