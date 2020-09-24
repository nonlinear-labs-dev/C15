package com.nonlinearlabs.client.world.maps.presets.html;

import com.google.gwt.core.client.GWT;
import com.google.gwt.dom.client.DataTransfer.DropEffect;
import com.google.gwt.event.dom.client.ClickEvent;
import com.google.gwt.event.dom.client.DragEndEvent;
import com.google.gwt.event.dom.client.DragEvent;
import com.google.gwt.event.dom.client.DragLeaveEvent;
import com.google.gwt.event.dom.client.DragOverEvent;
import com.google.gwt.event.dom.client.DragStartEvent;
import com.google.gwt.event.dom.client.DropEvent;
import com.google.gwt.user.client.ui.HTMLPanel;
import com.nonlinearlabs.client.presenters.PresetPresenterProviders;
import com.nonlinearlabs.client.useCases.BankUseCases;
import com.nonlinearlabs.client.world.maps.presets.html.PresetManagerUI.DragDropData;

class PresetUI extends HTMLPanel {
    class DropZone extends HTMLPanel {
        DropZone() {
            super("");
            getElement().addClassName("drop-zone");

            addDomHandler(e -> {
                String type = PresetManagerUI.get().getDragDropData().type;
                boolean isBank = type == "bank";
                boolean isPreset = type == "preset";
                boolean isPresets = type == "presets";

                if (isBank || isPreset || isPresets)
                    getElement().addClassName("drop-target");
                else
                    getElement().removeClassName("drop-target");

                e.preventDefault();
                e.stopPropagation();
                e.getDataTransfer().setDropEffect(DropEffect.MOVE);

                GWT.log("DragOverEvent Preset");

            }, DragOverEvent.getType());

            addDomHandler(e -> {
                getElement().removeClassName("drop-target");
                e.preventDefault();
                e.stopPropagation();
                GWT.log("DragEndEvent Preset");
            }, DragEndEvent.getType());

            addDomHandler(e -> {
                getElement().removeClassName("drop-target");
                e.preventDefault();
                e.stopPropagation();
                GWT.log("DragLeaveEvent Preset");
            }, DragLeaveEvent.getType());

            addDomHandler(e -> {
                getElement().removeClassName("drop-target");
                e.preventDefault();
                e.stopPropagation();
                GWT.log("DragEvent Preset");
            }, DragEvent.getType());
        }
    }

    class Above extends DropZone {

        public Above(String uuid) {
            getElement().addClassName("above");

            addDomHandler(e -> {
                getElement().removeClassName("drop-target");
                e.preventDefault();
                e.stopPropagation();
                DragDropData dnd = PresetManagerUI.get().getDragDropData();
                BankUseCases.get().dropAbove(uuid, dnd.type, dnd.data);
            }, DropEvent.getType());
        }
    }

    class Middle extends DropZone {
        public Middle(String uuid) {
            getElement().addClassName("middle");

            addDomHandler(e -> {
                getElement().removeClassName("drop-target");
                e.preventDefault();
                e.stopPropagation();
                DragDropData dnd = PresetManagerUI.get().getDragDropData();
                BankUseCases.get().dropOn(uuid, dnd.type, dnd.data);
            }, DropEvent.getType());
        }
    }

    class Below extends DropZone {
        public Below(String uuid) {
            getElement().addClassName("below");

            addDomHandler(e -> {
                getElement().removeClassName("drop-target");
                e.preventDefault();
                e.stopPropagation();
                DragDropData dnd = PresetManagerUI.get().getDragDropData();
                BankUseCases.get().dropBelow(uuid, dnd.type, dnd.data);
            }, DropEvent.getType());
        }
    }

    PresetUI(String uuid) {
        super("");

        getElement().addClassName("preset");
        getElement().setAttribute("draggable", "true");
        getElement().setId(uuid);

        NumberUI number = new NumberUI();
        NameUI name = new NameUI();

        add(number);
        add(name);

        add(new Above(uuid));
        add(new Middle(uuid));
        add(new Below(uuid));

        PresetPresenterProviders.get().register(uuid, presenter -> {
            number.setText(presenter.number);
            name.setText(presenter.name);

            switchClassName("selected", presenter.selected);
            switchClassName("loaded", presenter.loaded);

            return true;
        });

        addDomHandler(c -> {
            BankUseCases.get().selectPreset(uuid);
        }, ClickEvent.getType());

        addDomHandler(e -> {
            GWT.log("DragStartEvent Preset");
            e.getDataTransfer().setData("preset", getElement().getId());
            e.getDataTransfer().setDragImage(getElement(), 10, 10);
            e.stopPropagation();
            PresetManagerUI.get().setDragDropData("preset", getElement().getId());
        }, DragStartEvent.getType());

    }

    void switchClassName(String name, boolean set) {
        if (set)
            getElement().addClassName(name);
        else
            getElement().removeClassName(name);
    }
}