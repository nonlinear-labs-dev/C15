package com.nonlinearlabs.client.world.overlay.belt.presets.html;

import com.google.gwt.core.client.GWT;
import com.google.gwt.dom.client.Style.Unit;
import com.google.gwt.event.dom.client.MouseDownEvent;
import com.google.gwt.uibinder.client.UiBinder;
import com.google.gwt.uibinder.client.UiFactory;
import com.google.gwt.uibinder.client.UiField;
import com.google.gwt.user.client.ui.Button;
import com.google.gwt.user.client.ui.Composite;
import com.google.gwt.user.client.ui.HTMLPanel;
import com.google.gwt.user.client.ui.Label;
import com.google.gwt.user.client.ui.RootPanel;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.presenters.DeviceSettings;
import com.nonlinearlabs.client.presenters.DeviceSettingsProvider;
import com.nonlinearlabs.client.presenters.EditBufferPresenterProvider;
import com.nonlinearlabs.client.presenters.PresetManagerPresenter;
import com.nonlinearlabs.client.presenters.PresetManagerPresenterProvider;
import com.nonlinearlabs.client.useCases.PresetManagerUseCases;
import com.nonlinearlabs.client.useCases.SystemSettings;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.overlay.BankInfoDialog;
import com.nonlinearlabs.client.world.overlay.ContextMenu;
import com.nonlinearlabs.client.world.overlay.Overlay;
import com.nonlinearlabs.client.world.overlay.PresetInfoDialog;
import com.nonlinearlabs.client.world.overlay.belt.presets.CombinedBankContextMenu;
import com.nonlinearlabs.client.world.overlay.belt.presets.PresetContextMenu;
import com.nonlinearlabs.client.world.overlay.html.presetSearch.PresetSearchDialog;

public class BeltPresets extends Composite {

    interface BeltPresetsUIBinder extends UiBinder<HTMLPanel, BeltPresets> {
    }

    private static BeltPresetsUIBinder ourUiBinder = GWT.create(BeltPresetsUIBinder.class);

    @UiField
    Button showPresetMenu, showPresetInfo, showPresetSearch;

    @UiField
    Button showBankMenu, showBankInfo;

    @UiField
    Button showStoreMenu, storeSelectMode, store;

    @UiField
    Button previousPreset, nextPreset, loadPreset;

    @UiField
    Label numPresets, loadedPresetNumber;

    @UiField
    Button directLoad;

    private PresetManagerPresenter presetManager;
    public DeviceSettings deviceSettings;

    public BeltPresets() {
        initWidget(ourUiBinder.createAndBindUi(this));
        RootPanel.get().add(this);

        showPresetMenu.addClickHandler(v -> showPresetContextMenu(new Position(v)));
        showBankMenu.addClickHandler(v -> showBankContextMenu(new Position(v)));
        showPresetInfo.addClickHandler(v -> PresetInfoDialog.toggle());
        showBankInfo.addClickHandler(v -> BankInfoDialog.toggle());
        showPresetSearch.addClickHandler(v -> PresetSearchDialog.toggle());
        showStoreMenu.addClickHandler(v -> showStoreMenu(new Position(v)));
        storeSelectMode.addClickHandler(v -> PresetManagerUseCases.get().toggleStoreSelectMode());
        store.addClickHandler(v -> PresetManagerUseCases.get().storePreset());

        previousPreset.addClickHandler(v -> PresetManagerUseCases.get().selectPreviousPreset());
        nextPreset.addClickHandler(v -> PresetManagerUseCases.get().selectNextPreset());
        loadPreset.addClickHandler(v -> PresetManagerUseCases.get().loadSelectedPreset());
        directLoad.addClickHandler(v -> SystemSettings.get().toggleDirectLoad());

        PresetManagerPresenterProvider.get().onChange(p -> {
            presetManager = p;
            showPresetMenu.setEnabled(presetManager.hasPresets);
            showBankInfo.setEnabled(presetManager.hasPresets);
            showPresetSearch.setEnabled(presetManager.hasPresets);
            storeSelectMode.setEnabled(presetManager.hasPresets);

            if (presetManager.inStoreSelectMode && presetManager.hasPresets)
                storeSelectMode.addStyleName("active");
            else
                storeSelectMode.removeStyleName("active");

            if (presetManager.directLoad)
                directLoad.addStyleName("active");
            else
                directLoad.removeStyleName("active");

            previousPreset.setEnabled(presetManager.canSelectPrevPreset);
            nextPreset.setEnabled(presetManager.canSelectNextPreset);

            numPresets.setText(presetManager.numPresetsInCurrentBank);
            loadedPresetNumber.setText(presetManager.loadedPresetNumber);
            return true;
        });

        EditBufferPresenterProvider.get().onChange(v -> {
            loadPreset.setEnabled(v.isAnyParameterChanged);
            return true;
        });

        DeviceSettingsProvider.get().onChange(p -> {
            deviceSettings = p;
            showStoreMenu.setStyleName(deviceSettings.presetStoreMode.toString());
            showStoreMenu.addStyleName("show-menu");
            return true;
        });

        addDomHandler(e -> {
            Overlay o = getOverlay();
            o.removeExistingContextMenus();

        }, MouseDownEvent.getType());

    }

    private void showStoreMenu(Position position) {

        showMenu(position, new StoreOptionsContextMenu(this, getOverlay()));
    }

    private void showPresetContextMenu(Position position) {
        showMenu(position, new PresetContextMenu(getOverlay(), presetManager.selectedPreset));
    }

    private void showBankContextMenu(Position position) {
        showMenu(position, new CombinedBankContextMenu(getOverlay(), presetManager.selectedBank));
    }

    private void showMenu(Position p, ContextMenu menu) {
        Overlay o = getOverlay();
        o.removeExistingContextMenus();
        o.setContextMenu(p, menu);
    }

    private Overlay getOverlay() {
        return NonMaps.get().getNonLinearWorld().getViewport().getOverlay();
    }

    @UiFactory
    BankUI makeBankUI() {
        return new BankUI();
    }

    public void syncPosition(Rect r) {
        getElement().getStyle().setLeft(r.getLeft(), Unit.PX);
        getElement().getStyle().setTop(r.getTop(), Unit.PX);
        getElement().getStyle().setWidth(r.getWidth(), Unit.PX);
        getElement().getStyle().setHeight(r.getHeight(), Unit.PX);
    }
}
