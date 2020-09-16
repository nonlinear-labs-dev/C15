package com.nonlinearlabs.client.world.maps.presets.html;

import com.google.gwt.event.dom.client.ClickEvent;
import com.google.gwt.event.dom.client.ClickHandler;
import com.google.gwt.event.dom.client.HasClickHandlers;
import com.google.gwt.event.shared.HandlerRegistration;
import com.google.gwt.user.client.ui.HTMLPanel;
import com.nonlinearlabs.client.presenters.PresetPresenterProviders;
import com.nonlinearlabs.client.useCases.BankUseCases;

class PresetUI extends HTMLPanel implements HasClickHandlers {
    PresetUI(String uuid) {
        super("");

        getElement().addClassName("preset");
        getElement().setId(uuid);

        NumberUI number = new NumberUI();
        NameUI name = new NameUI();

        add(number);
        add(name);

        PresetPresenterProviders.get().register(uuid, presenter -> {
            number.setText(presenter.number);
            name.setText(presenter.name);

            if (presenter.selected)
                getElement().addClassName("selected");
            else
                getElement().removeClassName("selected");

            if (presenter.loaded)
                getElement().addClassName("loaded");
            else
                getElement().removeClassName("loaded");

            return true;
        });

        addClickHandler(c -> {
            BankUseCases.get().selectPreset(uuid);
        });

    }

    @Override
    public HandlerRegistration addClickHandler(ClickHandler handler) {
        return addDomHandler(handler, ClickEvent.getType());
    }

 
}