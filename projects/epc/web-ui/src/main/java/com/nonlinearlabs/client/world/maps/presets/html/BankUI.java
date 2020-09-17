package com.nonlinearlabs.client.world.maps.presets.html;

import com.google.gwt.core.client.GWT;
import com.google.gwt.dom.client.Style.Unit;
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

        BankHeaderUI header = new BankHeaderUI();
        PresetListUI presets = new PresetListUI();
        add(header);
        add(presets);

        BankPresenterProviders.get().register(uuid, presenter -> {
            header.setText(presenter.name);
            presets.syncPresets(presenter.presets);

            getElement().getStyle().setLeft(presenter.x / NonMaps.devicePixelRatio, Unit.PX);
            getElement().getStyle().setTop(presenter.y / NonMaps.devicePixelRatio, Unit.PX);

            return true;
        });

        addDomHandler(e -> {
            e.setData("bank", getElement().getId());
            GWT.log("DragStartEvent Bank");

            e.getDataTransfer().setData("bank", getElement().getId());
            e.getDataTransfer().setDragImage(getElement(), 10, 10);
        }, DragStartEvent.getType());

    }
}