package com.nonlinearlabs.client.world.maps.presets.html;

import com.google.gwt.event.dom.client.DropEvent;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.dataModel.presetManager.PresetManagerModel.DragDataType;
import com.nonlinearlabs.client.presenters.PresetManagerPresenterProvider;
import com.nonlinearlabs.client.useCases.BankUseCases;
import com.nonlinearlabs.client.world.maps.NonPosition;
import com.nonlinearlabs.client.world.maps.NonRect;

class Tape extends DropZone {

    private String uuid;
    private BankUseCases.TapePosition position;

    public Tape(String uuid, BankUseCases.TapePosition position) {
        super();
        this.uuid = uuid;
        this.position = position;

        getElement().addClassName("tape");
        getElement().addClassName(position.toString().toLowerCase());

        addDomHandler(e -> {
            getElement().removeClassName("drop-target");

            var dndType = PresetManagerPresenterProvider.get().getPresenter().dndType;

            if (dndType == DragDataType.Bank) {
                e.preventDefault();
                e.stopPropagation();
                NonRect viewPortRect = NonMaps.get().getNonLinearWorld().getViewport().getNonPosition();
                double z = NonMaps.get().getNonLinearWorld().getCurrentZoom();
                double x = z * viewPortRect.getLeft() / NonMaps.devicePixelRatio;
                double y = z * viewPortRect.getTop() / NonMaps.devicePixelRatio;
                double bx = NonMaps.devicePixelRatio * (e.getNativeEvent().getClientX() + x) / z;
                double by = NonMaps.devicePixelRatio * (e.getNativeEvent().getClientY() + y) / z;
                BankUseCases.get().dropDock(uuid, position, new NonPosition(bx, by));
            }
        }, DropEvent.getType());
    }

    public String getUuid() {
        return uuid;
    }

    public BankUseCases.TapePosition getPosition() {
        return position;
    }

    public void addDropTargetClass() {
        getElement().addClassName("maybe-drop-target");
    }

    public void removeDropTargetClass() {
        getElement().removeClassName("maybe-drop-target");
    }

}