package com.nonlinearlabs.client.world.maps.presets.html;

import java.util.ArrayList;
import java.util.HashMap;

import com.google.gwt.user.client.ui.HTMLPanel;
import com.google.gwt.user.client.ui.Widget;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.presenters.PresetManagerPresenterProvider;
import com.nonlinearlabs.client.world.maps.NonRect;

class PresetManagerPane extends HTMLPanel {

    public PresetManagerPane() {
        super("");

        PresetManagerPresenterProvider.get().register(p -> {
            sync(p.banks);
            return isAttached();
        });
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