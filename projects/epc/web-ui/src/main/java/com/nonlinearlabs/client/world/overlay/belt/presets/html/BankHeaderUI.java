package com.nonlinearlabs.client.world.overlay.belt.presets.html;

import com.google.gwt.event.dom.client.ContextMenuEvent;
import com.google.gwt.event.dom.client.DropEvent;
import com.google.gwt.user.client.ui.Button;
import com.google.gwt.user.client.ui.Image;
import com.google.gwt.user.client.ui.Label;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.dataModel.setup.SetupModel;
import com.nonlinearlabs.client.dataModel.setup.SetupModel.BooleanValues;
import com.nonlinearlabs.client.presenters.BankPresenterProviders;
import com.nonlinearlabs.client.presenters.PresetManagerPresenter;
import com.nonlinearlabs.client.presenters.PresetManagerPresenterProvider;
import com.nonlinearlabs.client.useCases.BankUseCases;
import com.nonlinearlabs.client.useCases.PresetManagerUseCases;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.html.DropZone;
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

    PresetManagerPresenter pmPresenter;

    BankHeaderUI() {
        getElement().addClassName("header");

        var text = new Label();
        var prevButton = new Button();
        var nextButton = new Button();

        add(prevButton);
        add(text);
        add(nextButton);

        prevButton.getElement().addClassName("prev-bank");
        nextButton.getElement().addClassName("next-bank");
        text.getElement().addClassName("bank-name");

        var leftImg = new Image("images/Bank_Left_Active.svg");
        prevButton.getElement().appendChild(leftImg.getElement());

        var rightImg = new Image("images/Bank_Right_Active.svg");
        nextButton.getElement().appendChild(rightImg.getElement());

        PresetManagerPresenterProvider.get().onChange(p -> {
            pmPresenter = p;

            nextButton.setEnabled(pmPresenter.canSelectNextBank);
            prevButton.setEnabled(pmPresenter.canSelectPrevBank);

            BankPresenterProviders.get().register(p.selectedBank, q -> {
                if (q.uuid != p.selectedBank)
                    return false;

                text.setText(q.name);
                return true;
            });

            return true;
        });

        prevButton.addClickHandler(v -> PresetManagerUseCases.get().selectPreviousBank());
        nextButton.addClickHandler(v -> PresetManagerUseCases.get().selectNextBank());

        addDomHandler(e -> {
            getElement().removeClassName("drop-target");
            e.preventDefault();
            e.stopPropagation();
            BankUseCases.get().dropOnBank(pmPresenter.selectedBank);
        }, DropEvent.getType());

        addDomHandler(e -> {
            e.stopPropagation();
            e.preventDefault();

            if (PresetManagerPresenterProvider.get().getValue().inStoreSelectMode)
                return;

            boolean showContextMenus = SetupModel.get().localSettings.contextMenus.getValue() == BooleanValues.on;

            if (showContextMenus) {
                Overlay o = NonMaps.theMaps.getNonLinearWorld().getViewport().getOverlay();
                o.setContextMenu(new Position(e.getNativeEvent()),
                        new MapsBankContextMenu(o, pmPresenter.selectedBank));
            }

        }, ContextMenuEvent.getType());
    }

}