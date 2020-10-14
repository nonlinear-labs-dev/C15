package com.nonlinearlabs.client.world.maps.presets.html;

import java.util.HashMap;

import com.google.gwt.dom.client.Style.Unit;
import com.google.gwt.event.dom.client.DragEndEvent;
import com.google.gwt.event.dom.client.DragStartEvent;
import com.google.gwt.user.client.ui.HTMLPanel;
import com.google.gwt.user.client.ui.Widget;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.dataModel.presetManager.PresetManagerModel.DragDataType;
import com.nonlinearlabs.client.useCases.BankUseCases.TapePosition;
import com.nonlinearlabs.client.useCases.PresetManagerUseCases;
import com.nonlinearlabs.client.world.PointerEvent;

class BankUI extends HTMLPanel {

    class BankContent extends HTMLPanel {
        public BankContent(String uuid) {
            super("");
            getElement().addClassName("content");
            add(new BankHeaderUI(uuid));
            add(new PresetListUI(uuid));
        }
    }

    class NestedBanks extends HTMLPanel {
        NestedBanks() {
            super("");
            getElement().addClassName("nested");
        }

        public void addNestedBanks(HashMap<String, BankUI> widgets) {
            for (Widget w : this.getChildren()) {
                if (w instanceof BankUI) {
                    BankUI b = (BankUI) w;
                    widgets.put(b.getElement().getAttribute("id"), b);
                    b.addNestedBanks(widgets);
                }
            }
        }
    }

    class HorizontalLayout extends HTMLPanel {
        private NestedBanks nested;

        public HorizontalLayout(String uuid) {
            super("");
            getElement().addClassName("horizontal");

            add(new BankContent(uuid));
            add(new Tape(uuid, TapePosition.East));
            add(nested = new NestedBanks());
        }

        public NestedBanks getNestedBanks() {
            return nested;
        }
    }

    class VerticalLayout extends HTMLPanel {

        private HorizontalLayout layout;
        private NestedBanks nested;

        public VerticalLayout(String uuid) {
            super("");
            getElement().addClassName("vertical");
            add(layout = new HorizontalLayout(uuid));
            add(new Tape(uuid, TapePosition.South));
            add(nested = new NestedBanks());
        }

        public NestedBanks getNestedBanks(String direction) {
            if (direction == "top")
                return nested;

            return layout.getNestedBanks();
        }
    }

    private VerticalLayout layout;

    public BankUI(String uuid) {
        super("");

        getElement().addClassName("bank");
        getElement().setAttribute("draggable", "true");
        getElement().setId(uuid);

        add(layout = new VerticalLayout(uuid));

        addDomHandler(e -> {
            var dnd = PresetManagerUseCases.get().setDragDropData(DragDataType.Bank, getElement().getId());
            if (dnd != null) {
                e.getDataTransfer().setData(dnd.type.toString(), dnd.data);
                e.getDataTransfer().setDragImage(getElement(), 0, 0);
                e.stopPropagation();
            }
        }, DragStartEvent.getType());

        addDomHandler(e -> {
            e.stopPropagation();
            PresetManagerUseCases.get().resetDragDropData();
        }, DragEndEvent.getType());

        PointerEvent pointerDown = new PointerEvent("pointerdown");

        addDomHandler(c -> {
            NonMaps.get().getNonLinearWorld().getViewport().getOverlay().removeExistingContextMenus();
        }, pointerDown.eventType);
    }

    enum Nesting {
        None, Horizontally, Vertically
    }

    public void attachTo(HTMLPanel newParent, double x, double y, Nesting nesting) {
        if (getParent() != newParent) {
            removeFromParent();
            newParent.add(this);
        }

        getElement().getStyle().setLeft(x / NonMaps.devicePixelRatio, Unit.PX);
        getElement().getStyle().setTop(y / NonMaps.devicePixelRatio, Unit.PX);

        if (nesting == Nesting.None)
            getElement().removeClassName("nested");
        else
            getElement().addClassName("nested");

        if (nesting == Nesting.Horizontally)
            getElement().addClassName("horizontally");
        else
            getElement().removeClassName("horizontally");

        if (nesting == Nesting.Vertically)
            getElement().addClassName("vertically");
        else
            getElement().removeClassName("vertically");

    }

    public void addNestedBanks(HashMap<String, BankUI> widgets) {
        layout.nested.addNestedBanks(widgets);
        layout.layout.nested.addNestedBanks(widgets);
    }

    public NestedBanks getNestedBanks(String direction) {
        return layout.getNestedBanks(direction);
    }
}