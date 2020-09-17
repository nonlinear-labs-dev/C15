package com.nonlinearlabs.client.world.maps.presets.html;

import java.util.ArrayList;
import java.util.HashMap;

import com.google.gwt.core.client.GWT;
import com.google.gwt.dom.client.DataTransfer.DropEffect;
import com.google.gwt.event.dom.client.DragEndEvent;
import com.google.gwt.event.dom.client.DragEvent;
import com.google.gwt.event.dom.client.DragLeaveEvent;
import com.google.gwt.event.dom.client.DragOverEvent;
import com.google.gwt.event.dom.client.DropEvent;
import com.google.gwt.user.client.ui.HTMLPanel;
import com.google.gwt.user.client.ui.RootPanel;
import com.google.gwt.user.client.ui.Widget;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.presenters.PresetManagerPresenterProvider;
import com.nonlinearlabs.client.world.maps.NonRect;

/*
Features we have to support:

Prio 1
- move bank by dragging header

- Select multiple presets
- Attach/group banks
- StoreSelectMode
- next/prev bank/preset
- search
- grid
- drag preset
- drop preset above/on/below for move/copy
- drop presets above/on/below for move/copy
- drop bank above/on/below 

- drop preset/bank on header
- bank context menu
- preset context menu
- bank selection state
- preset selection state
- preset loaded state
- preset sound type
- preset color tag
- compare presets
- multiple presets drag 

Prio 2
- highlight presets matching a search
- drop "Store"-button above/on/below 
- drop "Store"-button on header

Prio 3
- Move all banks
- move to selected preset
- collapse/expand bank
- collapse/expand all banks
- multiple presets drag with fancy graphic
*/

public class PresetManagerUI extends HTMLPanel {

    public PresetManagerUI() {
        super("");

        getElement().addClassName("pm");
        getElement().setAttribute("draggable", "true");
        RootPanel.get("preset-manager").add(this);

        PresetManagerPresenterProvider.get().register(p -> {
            sync(p.banks);
            return true;
        });

        Widget parent = getParent();

        parent.addDomHandler(e -> {
            boolean isBank = !e.getData("bank").isEmpty();
            boolean isPreset = !e.getData("preset").isEmpty();
            boolean isPresets = !e.getData("presets").isEmpty();

            if (isBank || isPreset || isPresets)
                getElement().addClassName("drop-target");
            else
                getElement().removeClassName("drop-target");

            e.preventDefault();
            e.stopPropagation();
            e.getDataTransfer().setDropEffect(DropEffect.MOVE);

            GWT.log("DragOverEvent PM");

        }, DragOverEvent.getType());

        parent.addDomHandler(e -> {
            getElement().removeClassName("drop-target");
            e.preventDefault();
            e.stopPropagation();
            GWT.log("DropEvent PM");
        }, DropEvent.getType());

        parent.addDomHandler(e -> {
            getElement().removeClassName("drop-target");
            e.preventDefault();
            e.stopPropagation();
            GWT.log("DragEndEvent PM");
        }, DragEndEvent.getType());

        parent.addDomHandler(e -> {
            getElement().removeClassName("drop-target");
            e.preventDefault();
            e.stopPropagation();
            GWT.log("DragLeaveEvent PM");
        }, DragLeaveEvent.getType());

        parent.addDomHandler(e -> {
            getElement().removeClassName("drop-target");
            e.preventDefault();
            e.stopPropagation();
            GWT.log("DragEvent PM");
        }, DragEvent.getType());

    }

    private void sync(ArrayList<String> banks) {
        HashMap<String, BankUI> widgets = new HashMap<String, BankUI>();

        for (Widget w : this.getChildren()) {
            if (w instanceof BankUI) {
                BankUI b = (BankUI) w;
                widgets.put(b.getElement().getAttribute("id"), b);
            }
        }

        for (String uuid : banks) {
            if (!widgets.containsKey(uuid))
                add(new BankUI(uuid));

            widgets.remove(uuid);
        }

        widgets.forEach((k, v) -> remove(v));
    }

    public void syncPosition() {
        NonRect viewPortRect = NonMaps.get().getNonLinearWorld().getViewport().getNonPosition();

        double z = NonMaps.get().getNonLinearWorld().getCurrentZoom();
        double x = z * viewPortRect.getLeft() / NonMaps.devicePixelRatio;
        double y = z * viewPortRect.getTop() / NonMaps.devicePixelRatio;

        getElement().getStyle().setProperty("transformOrigin", "0 0");
        getElement().getStyle().setProperty("transform", "translate(" + -x + "px, " + -y + "px) scale(" + z + ")");
    }
}