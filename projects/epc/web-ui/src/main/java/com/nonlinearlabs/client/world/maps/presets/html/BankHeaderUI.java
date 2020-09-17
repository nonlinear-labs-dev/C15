package com.nonlinearlabs.client.world.maps.presets.html;

import com.google.gwt.user.client.ui.Label;

class BankHeaderUI extends Label {
    BankHeaderUI() {
        getElement().addClassName("header");

        // addDomHandler(e -> {
        // e.getDataTransfer().setData("application/x-bank-uuid", getElement().getId());
        // GWT.log("DragStartEvent Header");
        // }, DragStartEvent.getType());

        // addDomHandler(e -> {
        // boolean isBank = e.getData("bank") != null;
        // boolean isPreset = e.getData("preset") != null;
        // boolean isPresets = e.getData("presets") != null;

        // if (isBank || isPreset || isPresets)
        // getElement().addClassName("drop-target");
        // else
        // getElement().removeClassName("drop-target");

        // GWT.log("DragOverEvent Header");
        // }, DragOverEvent.getType());

        // addDomHandler(e -> {
        // getElement().removeClassName("drop-target");
        // GWT.log("DragLeaveEvent Header");
        // }, DragLeaveEvent.getType());

        // addDomHandler(e -> {
        // getElement().removeClassName("drop-target");
        // GWT.log("DropEvent Header");
        // }, DropEvent.getType());

        // addDomHandler(e -> {
        // getElement().removeClassName("drop-target");
        // GWT.log("DropEndEvent Header");
        // }, DragEndEvent.getType());
    }
}