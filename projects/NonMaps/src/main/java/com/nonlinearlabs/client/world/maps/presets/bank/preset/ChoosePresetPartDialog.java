package com.nonlinearlabs.client.world.maps.presets.bank.preset;

import com.google.gwt.event.dom.client.ClickEvent;
import com.google.gwt.event.dom.client.ClickHandler;
import com.google.gwt.user.client.Window;
import com.google.gwt.user.client.ui.Button;
import com.google.gwt.user.client.ui.HTMLPanel;
import com.google.gwt.user.client.ui.Label;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.VoiceGroup;
import com.nonlinearlabs.client.useCases.EditBufferUseCases;
import com.nonlinearlabs.client.world.overlay.GWTDialog;

public class ChoosePresetPartDialog extends GWTDialog {

    static int modalPopupLeft = 0;
    static int modalPopupTop = 0;
    Preset preset = null;

    public ChoosePresetPartDialog(Preset p, VoiceGroup target) {
        setModal(true);
        setWidth("20em");
        addHeader("Part of Dual Preset?");

        preset = p;

        HTMLPanel panel = new HTMLPanel("");
        HTMLPanel buttons = new HTMLPanel("");
        panel.add(new Label("Which part of the dual preset do you want to load into EditBuffer part " + target.toString() + "?", true));

        Button part1, part2, cancelButton;

        buttons.add(part1 = new Button("Part I", new ClickHandler() {

            @Override
            public void onClick(ClickEvent arg0) {
                EditBufferUseCases.get().loadPresetPartIntoPart(preset.getUUID(), VoiceGroup.I, target);
                commit();
            }
        }));

        buttons.add(part2 = new Button("Part II", new ClickHandler() {

            @Override
            public void onClick(ClickEvent arg0) {
                EditBufferUseCases.get().loadPresetPartIntoPart(preset.getUUID(), VoiceGroup.II, target);
                commit();
            }
        }));

        buttons.add(cancelButton = new Button("Cancel", new ClickHandler() {

            @Override
            public void onClick(ClickEvent arg0) {
                commit();
            }
        }));

        part1.getElement().addClassName("modal-button-button");
        part2.getElement().addClassName("modal-button-button");
        cancelButton.getElement().addClassName("modal-button-button");
        buttons.getElement().addClassName("modal-button-div");
        panel.add(buttons);
        add(panel);


        setPopupPosition(Window.getClientWidth() / 2 - panel.getOffsetWidth(), Window.getClientHeight() / 2 - panel.getOffsetHeight());
        show();
    }

    @Override
    protected void setLastPopupPos(int popupLeft, int popupTop) {
        modalPopupLeft = popupLeft;
        modalPopupTop = popupTop;
    }

    @Override
    protected int getLastPopupPosTop() {
        return modalPopupTop;
    }

    @Override
    protected int getLastPopupPosLeft() {
        return modalPopupLeft;
    }

    @Override
    protected void commit() {
        hide();
        NonMaps.theMaps.captureFocus();
        NonMaps.theMaps.getNonLinearWorld().requestLayout();

        modalPopupLeft = getAbsoluteLeft();
        modalPopupTop = getAbsoluteTop();
    }

}
