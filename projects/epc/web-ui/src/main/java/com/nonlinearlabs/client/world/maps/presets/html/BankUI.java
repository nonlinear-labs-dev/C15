package com.nonlinearlabs.client.world.maps.presets.html;

import java.util.HashMap;

import com.google.gwt.dom.client.Style.Unit;
import com.google.gwt.event.dom.client.DragEndEvent;
import com.google.gwt.event.dom.client.DragStartEvent;
import com.google.gwt.user.client.ui.HTMLPanel;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.dataModel.presetManager.PresetManagerModel.DragDataType;
import com.nonlinearlabs.client.useCases.BankUseCases.TapePosition;
import com.nonlinearlabs.client.useCases.PresetManagerUseCases;

class BankUI extends HTMLPanel {

    class BankContent extends HTMLPanel {
        public BankContent(String uuid) {
            super("");
            getElement().addClassName("content");
            add(new BankHeaderUI(uuid));
            add(new PresetListUI(uuid));
        }
    }

    class HorizontalLayout extends HTMLPanel {
        private Tape east;

        public HorizontalLayout(String uuid) {
            super("");
            getElement().addClassName("horizontal");

            add(new Tape(uuid, TapePosition.West));
            add(new BankContent(uuid));
            add(east = new Tape(uuid, TapePosition.East));
        }

        public Tape getRightTape() {
            return east;
        }
    }

    class VerticalLayout extends HTMLPanel {

        private Tape south;
        private HorizontalLayout layout;

        public VerticalLayout(String uuid) {
            super("");
            getElement().addClassName("vertical");
            add(new Tape(uuid, TapePosition.North));
            add(layout = new HorizontalLayout(uuid));
            add(south = new Tape(uuid, TapePosition.South));
        }

        public Tape getTape(String direction) {
            if (direction == "top")
                return south;

            return layout.getRightTape();
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

    public Tape getTape(String direction) {
        return layout.getTape(direction);
    }

    public void addNestedBanks(HashMap<String, BankUI> widgets) {
        layout.south.addNestedBanks(widgets);
        layout.layout.east.addNestedBanks(widgets);
    }
}