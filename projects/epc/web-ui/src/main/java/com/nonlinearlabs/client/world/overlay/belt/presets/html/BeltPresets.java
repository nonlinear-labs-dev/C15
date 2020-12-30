package com.nonlinearlabs.client.world.overlay.belt.presets.html;

import com.google.gwt.core.client.GWT;
import com.google.gwt.dom.client.Style.Unit;
import com.google.gwt.uibinder.client.UiBinder;
import com.google.gwt.uibinder.client.UiFactory;
import com.google.gwt.user.client.ui.Composite;
import com.google.gwt.user.client.ui.HTMLPanel;
import com.google.gwt.user.client.ui.RootPanel;
import com.nonlinearlabs.client.world.Rect;

public class BeltPresets extends Composite {

    interface BeltPresetsUIBinder extends UiBinder<HTMLPanel, BeltPresets> {
    }

    private static BeltPresetsUIBinder ourUiBinder = GWT.create(BeltPresetsUIBinder.class);

    public BeltPresets() {
        initWidget(ourUiBinder.createAndBindUi(this));
        RootPanel.get().add(this);

        
        // getElement().addClassName("");
        // add(new BankUI());
    }

    @UiFactory BankUI makeBankUI() { 
        return new BankUI();
      }

    public void syncPosition(Rect r) {
        getElement().getStyle().setLeft(r.getLeft(), Unit.PX);
        getElement().getStyle().setTop(r.getTop(), Unit.PX);
        getElement().getStyle().setWidth(r.getWidth(), Unit.PX);
        getElement().getStyle().setHeight(r.getHeight(), Unit.PX);
    }
}
