package com.nonlinearlabs.client.world.maps.presets.html;

import com.google.gwt.event.dom.client.DropEvent;
import com.google.gwt.user.client.ui.RootPanel;
import com.nonlinearlabs.client.Millimeter;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.dataModel.presetManager.PresetManagerModel.DragDataType;
import com.nonlinearlabs.client.presenters.PresetManagerPresenter;
import com.nonlinearlabs.client.presenters.PresetManagerPresenterProvider;
import com.nonlinearlabs.client.useCases.PresetManagerUseCases;
import com.nonlinearlabs.client.world.maps.NonRect;

public class PresetManagerUI extends DropZone {
    static private PresetManagerUI thePresetManagerUI;

    private PresetManagerPane thePane;
    private PresetManagerPresenter presenter;

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

            NonRect viewPortRect = NonMaps.get().getNonLinearWorld().getViewport().getNonPosition();
            double z = NonMaps.get().getNonLinearWorld().getCurrentZoom();
            double x = z * viewPortRect.getLeft() / NonMaps.devicePixelRatio;
            double y = z * viewPortRect.getTop() / NonMaps.devicePixelRatio;
            double bx = NonMaps.devicePixelRatio * (e.getNativeEvent().getClientX() + x) / z;
            double by = NonMaps.devicePixelRatio * (e.getNativeEvent().getClientY() + y) / z;
            double margin = Millimeter.toPixels(10) * z / NonMaps.devicePixelRatio;

            PresetManagerUseCases.get().drop(bx - margin, by - margin);

        }, DropEvent.getType());

        PresetManagerPresenterProvider.get().register(presenter -> {
            this.presenter = presenter;
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