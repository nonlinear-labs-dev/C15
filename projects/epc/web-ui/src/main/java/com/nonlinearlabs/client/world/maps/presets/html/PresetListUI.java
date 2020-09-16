package com.nonlinearlabs.client.world.maps.presets.html;

import java.util.ArrayList;
import java.util.HashMap;

import com.google.gwt.user.client.ui.HTMLPanel;
import com.google.gwt.user.client.ui.Widget;

class PresetListUI extends HTMLPanel {

    public PresetListUI() {
        super("");
        getElement().addClassName("presets");

    }

    public void syncPresets(ArrayList<String> presets) {
        HashMap<String, PresetUI> widgets = new HashMap<String, PresetUI>();

        for (Widget w : getChildren()) {
            if (w instanceof PresetUI) {
                PresetUI p = (PresetUI) w;
                widgets.put(p.getElement().getAttribute("id"), p);
            }
        }

        for (String uuid : presets) {
            if (!widgets.containsKey(uuid))
                add(new PresetUI(uuid));

            widgets.remove(uuid);
        }

        widgets.forEach((k, v) -> remove(v));
    }
}