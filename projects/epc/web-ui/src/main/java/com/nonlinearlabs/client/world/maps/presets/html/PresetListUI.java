package com.nonlinearlabs.client.world.maps.presets.html;

import java.util.ArrayList;
import java.util.HashMap;

import com.google.gwt.user.client.ui.HTMLPanel;
import com.google.gwt.user.client.ui.Widget;
import com.nonlinearlabs.client.presenters.BankPresenterProviders;

class PresetListUI extends HTMLPanel {

    public PresetListUI(String bankUuid) {
        super("");
        getElement().addClassName("presets");
        BankPresenterProviders.get().register(bankUuid, p -> syncPresets(p.presets));
    }

    private boolean syncPresets(ArrayList<String> presets) {
        HashMap<String, PresetUI> widgets = new HashMap<String, PresetUI>();

        for (Widget w : getChildren()) {
            if (w instanceof PresetUI) {
                PresetUI p = (PresetUI) w;
                widgets.put(p.getElement().getAttribute("id"), p);
            }
        }

        widgets.forEach((k, v) -> remove(v));

        for (String uuid : presets) {
            if (!widgets.containsKey(uuid))
                add(new PresetUI(uuid));
            else
                add(widgets.get(uuid));

            widgets.remove(uuid);
        }
        return isAttached();
    }
}