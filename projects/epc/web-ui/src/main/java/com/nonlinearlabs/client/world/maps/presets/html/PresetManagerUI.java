package com.nonlinearlabs.client.world.maps.presets.html;

import java.util.ArrayList;
import java.util.HashMap;

import com.google.gwt.event.dom.client.DropEvent;
import com.google.gwt.user.client.ui.RootPanel;
import com.nonlinearlabs.client.Millimeter;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.ServerProxy.BankAndPosition;
import com.nonlinearlabs.client.dataModel.clipboard.ClipboardModel.DragDataType;
import com.nonlinearlabs.client.presenters.ClipboardPresenterProvider;
import com.nonlinearlabs.client.useCases.PresetManagerUseCases;
import com.nonlinearlabs.client.world.html.DropZone;

public class PresetManagerUI extends DropZone {
    static private PresetManagerUI thePresetManagerUI;

    private PresetManagerPane thePane;
 
    public PresetManagerUI() {
        super();

        thePresetManagerUI = this;
        RootPanel.get().add(this);

        getElement().addClassName("preset-manager");
        getElement().setId("preset-manager");
        add(thePane = new PresetManagerPane());

        addDomHandler(e -> {
            getElement().removeClassName("drop-target");

            e.preventDefault();
            e.stopPropagation();

            double z = NonMaps.get().getNonLinearWorld().getCurrentZoom();
            double bx = PositionMappers.clientXToNonX(e.getNativeEvent().getClientX());
            double by = PositionMappers.clientYToNonY(e.getNativeEvent().getClientY());
            double margin = Millimeter.toPixels(10) * z / NonMaps.devicePixelRatio;

            PresetManagerUseCases.get().drop(bx - margin, by - margin, (uuid) -> {
                for (var v : thePane.getBankWidgets().values()) {
                    if (v.getUuid() == uuid) {
                        HashMap<String, BankUI> banks = new HashMap<String, BankUI>();
                        banks.put(uuid, v);
                        v.addNestedBanks(banks);

                        ArrayList<BankAndPosition> ret = new ArrayList<BankAndPosition>();
                        // add the 'master' as first item, so playground knows which bank possibly to
                        // undock

                        var vRect = PositionMappers.getElementsNonRect(v.getElement());
                        ret.add(new BankAndPosition(uuid, vRect.getLeft(), vRect.getTop()));

                        for (var ui : banks.values()) {
                            if (ui.getUuid() != uuid) {
                                var rect = PositionMappers.getElementsNonRect(ui.getElement());
                                ret.add(new BankAndPosition(ui.getUuid(), rect.getLeft(), rect.getTop()));
                            }
                        }

                        return ret;
                    }

                }
                return null;
            });

        }, DropEvent.getType());

        ClipboardPresenterProvider.get().onChange(presenter -> {
            if (presenter.dndType == DragDataType.None)
                getElement().removeClassName("maybe-drop-target");
            else
                getElement().addClassName("maybe-drop-target");
            return true;
        });
    }

    public static PresetManagerUI get() {
        return thePresetManagerUI;
    }

    public void syncPosition() {
        thePane.syncPosition();
    }
}