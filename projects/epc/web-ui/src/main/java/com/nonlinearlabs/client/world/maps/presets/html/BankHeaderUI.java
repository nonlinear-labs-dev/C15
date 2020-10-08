package com.nonlinearlabs.client.world.maps.presets.html;

import com.google.gwt.event.dom.client.ClickEvent;
import com.google.gwt.event.dom.client.DropEvent;
import com.google.gwt.user.client.ui.Label;
import com.nonlinearlabs.client.presenters.BankPresenterProviders;
import com.nonlinearlabs.client.useCases.BankUseCases;
import com.nonlinearlabs.client.useCases.PresetManagerUseCases;

class BankHeaderUI extends DropZone {

    BankHeaderUI(String uuid) {
        getElement().addClassName("header");
        var text = new Label();
        add(text);

        addDomHandler(e -> {
            getElement().removeClassName("drop-target");
            e.preventDefault();
            e.stopPropagation();
            BankUseCases.get().dropOnBank(uuid);
        }, DropEvent.getType());

        addDomHandler(e -> {
            e.preventDefault();
            e.stopPropagation();
            PresetManagerUseCases.get().selectBank(uuid);
        }, ClickEvent.getType());

        BankPresenterProviders.get().register(uuid, presenter -> {
            text.setText(presenter.name);

            if (presenter.isSelected)
                getElement().addClassName("selected");
            else
                getElement().removeClassName("selected");

            return isAttached();
        });


       
    }

}