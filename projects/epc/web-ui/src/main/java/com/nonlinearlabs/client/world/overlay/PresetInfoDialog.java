package com.nonlinearlabs.client.world.overlay;

import com.google.gwt.user.client.ui.HTMLPanel;
import com.google.gwt.user.client.ui.RootPanel;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.presenters.EditBufferPresenterProvider;
import com.nonlinearlabs.client.presenters.PresetManagerPresenterProvider;
import com.nonlinearlabs.client.presenters.PresetPresenter;
import com.nonlinearlabs.client.presenters.PresetPresenterProviders;
import com.nonlinearlabs.client.world.overlay.InfoDialog.PresetInfoWidget;

public class PresetInfoDialog extends GWTDialog {

	public static PresetInfoDialog theDialog;
	public static PresetInfoWidget presetInfoPage = null;

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

	public PresetPresenter getCurrentPreset() {
		var uuid = PresetManagerPresenterProvider.get().getPresenter().selectedPreset;
		return PresetPresenterProviders.get().getPresenter(uuid);
	}

	public PresetPresenter getEditBuffer() {
		var uuid = EditBufferPresenterProvider.getPresenter().loadedPresetUUID;
		return PresetPresenterProviders.get().getPresenter(uuid);
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

		presetInfoPage.updateInfo(getCurrentPreset(), false);
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
			if (PresetManagerPresenterProvider.get().getPresenter().hasPresets)
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

	private void updateInfo(String uuid) {
		presetInfoPage.updateInfo(PresetPresenterProviders.get().getPresenter(uuid), true);
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
		update(PresetManagerPresenterProvider.get().getPresenter().selectedPreset);
	}
}