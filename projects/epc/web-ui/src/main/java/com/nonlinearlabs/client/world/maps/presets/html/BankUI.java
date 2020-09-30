package com.nonlinearlabs.client.world.maps.presets.html;

import com.google.gwt.dom.client.Style.Unit;
import com.google.gwt.event.dom.client.DragEndEvent;
import com.google.gwt.event.dom.client.DragStartEvent;
import com.google.gwt.user.client.ui.HTMLPanel;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.presenters.BankPresenterProviders;

class BankUI extends HTMLPanel {

    public BankUI(String uuid) {
        super("");
        getElement().addClassName("bank");
        getElement().setAttribute("draggable", "true");
        getElement().setId(uuid);

        BankHeaderUI header = new BankHeaderUI(uuid);
        PresetListUI presets = new PresetListUI();
        add(header);
        add(presets);

        BankPresenterProviders.get().register(uuid, presenter -> {
            header.setText(presenter.name);
            presets.syncPresets(presenter.presets);

            getElement().getStyle().setLeft(presenter.x / NonMaps.devicePixelRatio, Unit.PX);
            getElement().getStyle().setTop(presenter.y / NonMaps.devicePixelRatio, Unit.PX);
            return isAttached();
        });

        addDomHandler(e -> {
            e.getDataTransfer().setData("bank", getElement().getId());
            e.getDataTransfer().setDragImage(getElement(), 0, 0);
            e.stopPropagation();

            PresetManagerUI.get().setDragDropData("bank", getElement().getId());
        }, DragStartEvent.getType());

        addDomHandler(e -> {
            e.stopPropagation();
            PresetManagerUI.get().resetDragDropData();
        }, DragEndEvent.getType());

    }
}