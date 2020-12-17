package com.nonlinearlabs.client.world.maps.presets.html;

import java.util.HashMap;

import com.google.gwt.dom.client.Document;
import com.google.gwt.dom.client.Style.Position;
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
        private Tape tape;

        public HorizontalLayout(String uuid) {
            super("");
            getElement().addClassName("horizontal");

            add(new BankContent(uuid));
            add(tape = new Tape(uuid, TapePosition.East));
            add(nested = new NestedBanks());
        }

        public NestedBanks getNestedBanks() {
            return nested;
        }

        public Tape getTape() {
            return tape;
        }
    }

    class VerticalLayout extends HTMLPanel {

        private HorizontalLayout layout;
        private NestedBanks nested;
        private Tape tape;

        public VerticalLayout(String uuid) {
            super("");
            getElement().addClassName("vertical");
            add(layout = new HorizontalLayout(uuid));
            add(tape = new Tape(uuid, TapePosition.South));
            add(nested = new NestedBanks());
        }

        public NestedBanks getNestedBanks(String direction) {
            if (direction == "top")
                return nested;

            return layout.getNestedBanks();
        }

        public Tape getTape() {
            return tape;
        }

        public HorizontalLayout getLayout() {
            return layout;
        }
    }

    private VerticalLayout layout;
    private String uuid;

    public BankUI(String uuid) {
        super("");
        this.uuid = uuid;

        getElement().addClassName("bank");
        getElement().setAttribute("draggable", "true");
        getElement().setId(uuid);

        add(layout = new VerticalLayout(uuid));

        addDomHandler(e -> {
            var dnd = PresetManagerUseCases.get().setDragDropData(DragDataType.Bank, getElement().getId());
            if (dnd != null) {
                double z = NonMaps.get().getNonLinearWorld().getCurrentZoom();

                var dragProxyContainer = Document.get().createDivElement();
                dragProxyContainer.setId("drag-drop-image");
                dragProxyContainer.setInnerHTML(getElement().getString());
                var dragProxy = dragProxyContainer.getFirstChildElement();

                dragProxy.getStyle().setLeft(0, Unit.PX);
                dragProxy.getStyle().setTop(0, Unit.PX);
                dragProxy.getStyle().setPosition(Position.RELATIVE);

                dragProxy.getStyle().setProperty("transformOrigin", "0 0");
                dragProxy.getStyle().setProperty("transform", "scale(" + z + ")");

                var widgets = new HashMap<String, BankUI>();
                widgets.put(uuid, this);
                addNestedBanks(widgets);

                double minX = Double.MAX_VALUE;
                double maxX = Double.MIN_VALUE;
                double minY = Double.MAX_VALUE;
                double maxY = Double.MIN_VALUE;

                for (var f : widgets.values()) {
                    var top = PositionMappers.getElementTop(f.getElement());
                    var left = PositionMappers.getElementLeft(f.getElement());
                    var bottom = top + PositionMappers.getElementHeight(f.getElement());
                    var right = left + PositionMappers.getElementWidth(f.getElement());
                    minX = Math.min(minX, left);
                    maxX = Math.max(maxX, right);
                    minY = Math.min(minY, top);
                    maxY = Math.max(maxY, bottom);
                }

                dragProxyContainer.getStyle().setWidth((maxX - minX) + 5, Unit.PX);
                dragProxyContainer.getStyle().setHeight((maxY - minY) + 5, Unit.PX);

                Document.get().getBody().appendChild(dragProxyContainer);

                e.getDataTransfer().setData(dnd.type.toString(), dnd.data);
                e.getDataTransfer().setDragImage(dragProxyContainer, 0, 0);
                e.stopPropagation();
            }
        }, DragStartEvent.getType());

        addDomHandler(e -> {
            e.stopPropagation();
            Document.get().getElementById("drag-drop-image").removeFromParent();
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

    public String getUuid() {
        return uuid;
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