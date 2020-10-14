package com.nonlinearlabs.client.world.maps.presets.html;

import com.google.gwt.event.dom.client.ClickEvent;
import com.google.gwt.event.dom.client.ContextMenuEvent;
import com.google.gwt.event.dom.client.DropEvent;
import com.google.gwt.user.client.ui.Label;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.dataModel.setup.SetupModel;
import com.nonlinearlabs.client.dataModel.setup.SetupModel.BooleanValues;
import com.nonlinearlabs.client.presenters.BankPresenterProviders;
import com.nonlinearlabs.client.useCases.BankUseCases;
import com.nonlinearlabs.client.useCases.PresetManagerUseCases;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.overlay.Overlay;
import com.nonlinearlabs.client.world.overlay.OverlayLayout;
import com.nonlinearlabs.client.world.overlay.belt.presets.BankContextMenu;

class BankHeaderUI extends DropZone {

    private class MapsBankContextMenu extends BankContextMenu {
        private MapsBankContextMenu(OverlayLayout parent, String bank) {
            super(parent, bank);
        }

        @Override
        protected boolean hasCollapse() {
            return true;
        }

        @Override
        protected boolean hasPaste() {
            return true;
        }

        @Override
        protected boolean hasBankCreationRights() {
            return false;
        }
    }

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

        addDomHandler(e -> {
            e.stopPropagation();
            e.preventDefault();

            // TODO!
            // if (isInStoreSelectMode())
            // return null;

            boolean showContextMenus = SetupModel.get().localSettings.contextMenus.getValue() == BooleanValues.on;

            if (showContextMenus) {
                Overlay o = NonMaps.theMaps.getNonLinearWorld().getViewport().getOverlay();

                // boolean isInMultiSel = isSelectedInMultiplePresetSelectionMode();

                // if (isInMultiSel || (!isInMultiSel && !isInMultiplePresetSelectionMode()))
                o.setContextMenu(new Position(e.getNativeEvent()), new MapsBankContextMenu(o, uuid));
            }

        }, ContextMenuEvent.getType());

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