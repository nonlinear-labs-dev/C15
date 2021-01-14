package com.nonlinearlabs.client.world.maps.presets.html;

import java.util.ArrayList;
import java.util.HashMap;

import com.google.gwt.dom.client.Document;
import com.google.gwt.user.client.ui.HTMLPanel;
import com.google.gwt.user.client.ui.Widget;
import com.nonlinearlabs.client.Millimeter;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.dataModel.presetManager.Bank.AbsolutePosition;
import com.nonlinearlabs.client.dataModel.presetManager.Bank.Position;
import com.nonlinearlabs.client.dataModel.presetManager.Bank.RelativePosition;
import com.nonlinearlabs.client.dataModel.setup.SetupModel;
import com.nonlinearlabs.client.presenters.LocalSettingsProvider;
import com.nonlinearlabs.client.presenters.PresetManagerPresenterProvider;
import com.nonlinearlabs.client.world.maps.NonRect;
import com.nonlinearlabs.client.world.maps.presets.html.BankUI.Nesting;
import com.nonlinearlabs.client.world.overlay.PresetInfoDialog;

class PresetManagerPane extends HTMLPanel {

    private String selectedPreset = "";

    public PresetManagerPane() {
        super("");

        PresetManagerPresenterProvider.get().onChange(p -> {
            sync(p.banks);
            setPositions(p.bankPositions, p.selectedBank);

            if (selectedPreset != p.selectedPreset) {
                selectedPreset = p.selectedPreset;

                if (PresetInfoDialog.isShown())
                    PresetInfoDialog.update(selectedPreset);

                if (LocalSettingsProvider.get().getValue().selectionAutoScroll.isOneOf(
                        SetupModel.SelectionAutoScroll.parameter_and_preset, SetupModel.SelectionAutoScroll.preset))
                    scrollToSelectedPreset();
            }

            return isAttached();
        });
    }

    private void scrollToSelectedPreset() {
        var p = Document.get().getElementById(selectedPreset);

        if (p != null) {
            var rect = PositionMappers.getElementsNonRect(p);
            var z = NonMaps.get().getNonLinearWorld().getCurrentZoom();
            double margin = Millimeter.toPixels(10) * z / NonMaps.devicePixelRatio;
            rect.enlargeBy(margin);
            rect.moveBy(-margin / 2, -margin / 2);
            NonMaps.get().getNonLinearWorld().scrollToShow(rect);
        }
    }

    private void setPositions(ArrayList<Position> bankPositions, String selectedBank) {
        var widgets = getBankWidgets();

        for (var pos : bankPositions) {
            if (pos instanceof RelativePosition) {
                var a = (RelativePosition) pos;
                var bank = widgets.get(a.bank);
                var master = widgets.get(a.attachedTo);

                if (bank != null && master != null) {
                    var tape = master.getNestedBanks(a.direction);
                    var nesting = a.direction == "left" ? Nesting.Horizontally : Nesting.Vertically;
                    bank.attachTo(tape, 0, 0, nesting);
                    continue;
                }
            } else {
                var a = (AbsolutePosition) pos;
                var bank = widgets.get(a.bank);
                if (bank != null) {
                    bank.attachTo(this, a.x, a.y, Nesting.None);
                }
            }
        }

        makeSelectedClusterTopMost(bankPositions, selectedBank, widgets);
    }

    private void makeSelectedClusterTopMost(ArrayList<Position> bankPositions, String selectedBank,
            HashMap<String, BankUI> widgets) {
        var rootOfSelectedPosition = findRootOfBank(bankPositions, selectedBank);
        if (rootOfSelectedPosition != null) {
            var rootOfSelected = widgets.get(rootOfSelectedPosition.bank);

            if (rootOfSelected != null) {
                rootOfSelected.removeFromParent();
                rootOfSelected.attachTo(this, rootOfSelectedPosition.x, rootOfSelectedPosition.y, Nesting.None);
            }
        }
    }

    private AbsolutePosition findRootOfBank(ArrayList<Position> bankPositions, String bank) {
        for (var p : bankPositions) {
            if (p.bank == bank) {
                if (p instanceof RelativePosition) {
                    var r = (RelativePosition) p;
                    return findRootOfBank(bankPositions, r.attachedTo);
                } else if (p instanceof AbsolutePosition) {
                    return (AbsolutePosition) p;
                }
            }
        }
        return null;
    }

    public HashMap<String, BankUI> getBankWidgets() {
        var widgets = new HashMap<String, BankUI>();

        for (Widget w : this.getChildren()) {
            if (w instanceof BankUI) {
                BankUI b = (BankUI) w;
                widgets.put(b.getElement().getAttribute("id"), b);

                b.addNestedBanks(widgets);
            }
        }

        return widgets;
    }

    private void sync(ArrayList<String> banks) {
        var widgets = getBankWidgets();

        for (String uuid : banks) {
            if (!widgets.containsKey(uuid))
                add(new BankUI(uuid));

            widgets.remove(uuid);
        }

        widgets.forEach((k, v) -> v.removeFromParent());
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