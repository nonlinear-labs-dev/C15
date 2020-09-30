package com.nonlinearlabs.client.world.maps.presets.html;

import com.google.gwt.event.dom.client.DropEvent;
import com.google.gwt.user.client.ui.RootPanel;
import com.nonlinearlabs.client.Millimeter;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.useCases.BankUseCases;
import com.nonlinearlabs.client.world.maps.NonRect;

/*
Features we have to support:

Done:
- move bank by dragging header
- drag preset
- drop preset above/on/below for move/copy
- drop preset/bank on header

Prio 1
- Select multiple presets
- Attach/group banks
- StoreSelectMode
- next/prev bank/preset
- search
- grid

- drop presets above/on/below for move/copy
- drop bank above/on/below 

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

public class PresetManagerUI extends DropZone {
    static private PresetManagerUI thePresetManagerUI;

    private PresetManagerPane thePane;

    class DragDropData {
        public DragDropData() {
        }

        public DragDropData(String type, String data) {
            this.type = type;
            this.data = data;
        }

        public String type = "";
        public String data = "";
    }

    DragDropData dragDropData = new DragDropData();

    public PresetManagerUI() {
        super();

        thePresetManagerUI = this;

        RootPanel.get().add(this);

        getElement().addClassName("preset-manager");
        add(thePane = new PresetManagerPane());

        addDomHandler(e -> {
            getElement().removeClassName("drop-target");
            e.preventDefault();
            e.stopPropagation();

            boolean isBank = getDragDropData().type == "bank";

            if (isBank) {
                NonRect viewPortRect = NonMaps.get().getNonLinearWorld().getViewport().getNonPosition();
                double z = NonMaps.get().getNonLinearWorld().getCurrentZoom();
                double x = z * viewPortRect.getLeft() / NonMaps.devicePixelRatio;
                double y = z * viewPortRect.getTop() / NonMaps.devicePixelRatio;
                double bx = NonMaps.devicePixelRatio * (e.getNativeEvent().getClientX() + x) / z;
                double by = NonMaps.devicePixelRatio * (e.getNativeEvent().getClientY() + y) / z;
                // TODO: margin is buggy
                double margin = Millimeter.toPixels(10) * z / NonMaps.devicePixelRatio;
                BankUseCases.get().move(getDragDropData().data, bx - margin, by - margin);
            }
        }, DropEvent.getType());
    }

    public static PresetManagerUI get() {
        return thePresetManagerUI;
    }

    public void setDragDropData(String type, String data) {
        dragDropData = new DragDropData(type, data);
        getElement().addClassName("maybe-drop-target");
    }

    public void resetDragDropData() {
        dragDropData = new DragDropData();
        getElement().removeClassName("maybe-drop-target");
    }

    public DragDropData getDragDropData() {
        return dragDropData;
    }

    public void syncPosition() {
        thePane.syncPosition();
    }
}