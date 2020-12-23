package com.nonlinearlabs.client.world.overlay.belt.presets.html;

import com.google.gwt.dom.client.DataTransfer.DropEffect;
import com.google.gwt.event.dom.client.ClickEvent;
import com.google.gwt.event.dom.client.ContextMenuEvent;
import com.google.gwt.event.dom.client.DragEndEvent;
import com.google.gwt.event.dom.client.DragStartEvent;
import com.google.gwt.event.dom.client.DropEvent;
import com.google.gwt.user.client.ui.HTMLPanel;
import com.google.gwt.user.client.ui.Label;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.SoundType;
import com.nonlinearlabs.client.dataModel.presetManager.Preset.Color;
import com.nonlinearlabs.client.dataModel.setup.SetupModel;
import com.nonlinearlabs.client.dataModel.setup.SetupModel.BooleanValues;
import com.nonlinearlabs.client.presenters.PresetManagerPresenterProvider;
import com.nonlinearlabs.client.presenters.PresetPresenterProviders;
import com.nonlinearlabs.client.useCases.BankUseCases;
import com.nonlinearlabs.client.useCases.PresetManagerUseCases;
import com.nonlinearlabs.client.useCases.PresetUseCases;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.html.DropZone;
import com.nonlinearlabs.client.world.overlay.Overlay;
import com.nonlinearlabs.client.world.overlay.belt.presets.PresetContextMenu;

class PresetUI extends HTMLPanel {
    class Above extends DropZone {

        public Above(String uuid) {
            getElement().addClassName("above");

            addDomHandler(e -> {
                getElement().removeClassName("drop-target");
                e.preventDefault();
                e.stopPropagation();
                boolean ctrl = e.getNativeEvent().getCtrlKey();
                DropEffect effect = ctrl ? DropEffect.COPY : DropEffect.MOVE;
                BankUseCases.get().dropAbove(uuid, effect);
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
                boolean ctrl = e.getNativeEvent().getCtrlKey();
                DropEffect effect = ctrl ? DropEffect.COPY : DropEffect.MOVE;
                BankUseCases.get().dropOn(uuid, effect);
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
                boolean ctrl = e.getNativeEvent().getCtrlKey();
                var effect = ctrl ? DropEffect.COPY : DropEffect.MOVE;
                BankUseCases.get().dropBelow(uuid, effect);
            }, DropEvent.getType());
        }
    }

    class ColorTagUI extends Label {

        public void setColor(Color color) {
            getElement().setClassName("color-tag");
            getElement().addClassName(color.name());
        }
    }

    class SoundTypeUI extends Label {

        public void setType(SoundType type) {
            getElement().setClassName("sound-type");
            getElement().addClassName(type.name());
        }
    }

    PresetUI(String uuid) {
        super("");

        getElement().addClassName("preset");
        getElement().setAttribute("draggable", "true");
        getElement().setId("belt-" + uuid);

        var color = new ColorTagUI();
        var number = new NumberUI();
        var name = new NameUI();
        var soundType = new SoundTypeUI();

        add(color);
        add(number);
        add(name);
        add(soundType);

        add(new Above(uuid));
        add(new Middle(uuid));
        add(new Below(uuid));

        PresetPresenterProviders.get().register(uuid, presenter -> {
            number.setText(presenter.paddedNumber);
            name.setText(presenter.name);

            switchClassName("selected", presenter.selected);
            switchClassName("loaded", presenter.loaded);
            color.setColor(presenter.color);
            soundType.setType(presenter.type);

            if (presenter.selected)
                getElement().scrollIntoView();

            return isAttached();
        });

        addDomHandler(c -> {
            if (c.isControlKeyDown())
                PresetManagerUseCases.get().toggleMultipleSelection(uuid);
            else
                BankUseCases.get().selectPreset(uuid);
        }, ClickEvent.getType());

        addDomHandler(e -> {
            var dnd = PresetUseCases.get().startDrag(uuid);
            if (dnd != null) {
                e.getDataTransfer().setData(dnd.type.toString(), dnd.data);
                e.getDataTransfer().setDragImage(getElement(), 10, 10);
                e.stopPropagation();
            }
        }, DragStartEvent.getType());

        addDomHandler(e -> {
            e.stopPropagation();
            PresetManagerUseCases.get().resetDragDropData();
        }, DragEndEvent.getType());

        addDomHandler(e -> {
            e.stopPropagation();
            e.preventDefault();

            var pm = PresetManagerPresenterProvider.get().getValue();

            if (pm.inStoreSelectMode)
                return;

            boolean showContextMenus = SetupModel.get().localSettings.contextMenus.getValue() == BooleanValues.on;

            if (showContextMenus) {
                Overlay o = NonMaps.theMaps.getNonLinearWorld().getViewport().getOverlay();

                var isInMultiSel = pm.multiSelection;
                var presetIsInMultiSel = pm.currentMultiSelection != null && pm.currentMultiSelection.contains(uuid);

                if (presetIsInMultiSel || !isInMultiSel)
                    o.setContextMenu(new Position(e.getNativeEvent()), new PresetContextMenu(o, uuid));
            }

        }, ContextMenuEvent.getType());
    }

    void switchClassName(String name, boolean set) {
        if (set)
            getElement().addClassName(name);
        else
            getElement().removeClassName(name);
    }
}