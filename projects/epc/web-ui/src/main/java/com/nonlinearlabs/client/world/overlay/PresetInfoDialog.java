package com.nonlinearlabs.client.world.overlay;

import com.google.gwt.user.client.ui.HTMLPanel;
import com.google.gwt.user.client.ui.RootPanel;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.presenters.PresetManagerPresenterProvider;
import com.nonlinearlabs.client.world.maps.presets.bank.preset.Preset;
import com.nonlinearlabs.client.world.overlay.InfoDialog.PresetInfoWidget;

public class PresetInfoDialog extends GWTDialog {

	public static PresetInfoDialog theDialog;
	public static PresetInfoWidget presetInfoPage = null;

	public Preset getCurrentPreset() {
		return NonMaps.get().getNonLinearWorld().getPresetManager().getSelectedPreset();
	}

	private PresetInfoDialog() {
		RootPanel.get().add(this);

		getElement().addClassName("preset-info-dialog");
		initalShow();
		setAnimationEnabled(true);
		setGlassEnabled(false);
		setModal(false);
		addHeader("Preset Info");
		addContent();
		super.pushDialogToFront();
	}

	public static Preset getEditBuffer() {
		return NonMaps.get().getNonLinearWorld().getPresetManager().getLoadedPreset();
	}

	private void addContent() {
		presetInfoPage = PresetInfoWidget.get();

		HTMLPanel pane = new HTMLPanel("div", "");
		pane.getElement().addClassName("preset-info-edit-buffer-dialog-pane");
		HTMLPanel content = new HTMLPanel("div", "");
		content.getElement().addClassName("content");
		HTMLPanel tabRest = new HTMLPanel("div", "");
		tabRest.getElement().addClassName("tab-rest");
		pane.add(content);
		content.add(presetInfoPage.panel);
		setWidget(pane);

		presetInfoPage.updateInfo(getCurrentPreset().getUUID(), false);
	}

	@Override
	protected void commit() {
		hide();
		theDialog = null;
		NonMaps.theMaps.captureFocus();
		NonMaps.theMaps.getNonLinearWorld().requestLayout();
	}

	public static void toggle() {
		if (theDialog != null) {
			theDialog.commit();
		} else {
			if (!NonMaps.theMaps.getNonLinearWorld().getPresetManager().isEmpty())
				theDialog = new PresetInfoDialog();
		}
	}

	public static boolean isShown() {
		return theDialog != null;
	}

	public static void update(String preset) {
		if (theDialog != null) {
			theDialog.updateInfo(preset);
		}
	}

	private void updateInfo(String preset) {
		presetInfoPage.updateInfo(preset, true);
		centerIfOutOfView();
	}

	private static int lastPopupLeft = -1;
	private static int lastPopupTop = -1;

	@Override
	protected void setLastPopupPos(int popupLeft, int popupTop) {
		lastPopupLeft = popupLeft;
		lastPopupTop = popupTop;
	}

	@Override
	protected int getLastPopupPosTop() {
		return lastPopupTop;
	}

	@Override
	protected int getLastPopupPosLeft() {
		return lastPopupLeft;
	}

	public static void update() {
		update(PresetManagerPresenterProvider.get().getPresenter().selectedPresetOfSelectedBank);
	}
}