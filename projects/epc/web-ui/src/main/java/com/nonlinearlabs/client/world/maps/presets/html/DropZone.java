package com.nonlinearlabs.client.world.maps.presets.html;

import com.google.gwt.dom.client.DataTransfer.DropEffect;
import com.google.gwt.event.dom.client.DragEndEvent;
import com.google.gwt.event.dom.client.DragEvent;
import com.google.gwt.event.dom.client.DragLeaveEvent;
import com.google.gwt.event.dom.client.DragOverEvent;
import com.google.gwt.user.client.ui.HTMLPanel;
import com.nonlinearlabs.client.dataModel.presetManager.PresetManagerModel.DragDataType;
import com.nonlinearlabs.client.presenters.PresetManagerPresenterProvider;

class DropZone extends HTMLPanel {
    DropZone() {
        super("");
        getElement().addClassName("drop-zone");

        addDomHandler(e -> {
            var type  = PresetManagerPresenterProvider.get().getPresenter().dndType;
            var isBank = type == DragDataType.Bank;
            var isPreset = type == DragDataType.Preset;
            var isPresets = type == DragDataType.Presets;

            if (isBank || isPreset || isPresets)
                getElement().addClassName("drop-target");
            else
                getElement().removeClassName("drop-target");

            e.preventDefault();
            e.stopPropagation();

        }, DragOverEvent.getType());

        addDomHandler(e -> {
            getElement().removeClassName("drop-target");
            e.preventDefault();
            e.stopPropagation();
        }, DragEndEvent.getType());

        addDomHandler(e -> {
            getElement().removeClassName("drop-target");
            e.preventDefault();
            e.stopPropagation();
        }, DragLeaveEvent.getType());

        addDomHandler(e -> {
            getElement().removeClassName("drop-target");
            e.preventDefault();
            e.stopPropagation();

            boolean ctrl = e.getNativeEvent().getCtrlKey();
            DropEffect effect = ctrl ? DropEffect.COPY : DropEffect.MOVE;
            e.getDataTransfer().setDropEffect(effect);
        }, DragEvent.getType());
    }
}