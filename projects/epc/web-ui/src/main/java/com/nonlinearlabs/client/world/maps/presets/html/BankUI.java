package com.nonlinearlabs.client.world.maps.presets.html;

import com.google.gwt.dom.client.Style.Unit;
import com.google.gwt.user.client.ui.HTMLPanel;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.presenters.BankPresenterProviders;

class BankUI extends HTMLPanel {
    public BankUI(String uuid) {
        super("");
        getElement().addClassName("bank");
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

    }

    public void syncPosition(double x, double y, double z) {

    }
}