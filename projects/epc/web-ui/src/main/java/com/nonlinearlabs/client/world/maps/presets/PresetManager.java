package com.nonlinearlabs.client.world.maps.presets;

import com.google.gwt.event.dom.client.KeyDownEvent;
import com.google.gwt.user.client.Window;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.SoundType;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.VoiceGroup;
import com.nonlinearlabs.client.presenters.PresetManagerPresenterProvider;
import com.nonlinearlabs.client.useCases.EditBufferUseCases;
import com.nonlinearlabs.client.useCases.PresetManagerUseCases;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.NonLinearWorld;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.presets.html.PresetManagerUI;
import com.nonlinearlabs.client.world.overlay.BankInfoDialog;
import com.nonlinearlabs.client.world.overlay.ParameterInfoDialog;
import com.nonlinearlabs.client.world.overlay.PresetInfoDialog;
import com.nonlinearlabs.client.world.overlay.belt.presets.PresetContextMenu;
import com.nonlinearlabs.client.world.overlay.html.presetSearch.PresetSearchDialog;

public class PresetManager extends MapsLayout {

	private PresetManagerUI theHtmlUI = new PresetManagerUI();

	public PresetManager(NonLinearWorld parent) {
		super(parent);

		EditBufferModel.get().soundType.onChange(type -> {
			if (type == SoundType.Single) {
				endLoadToPartMode();
				return true;
			}
			return true;
		});
	}

	public void startStoreSelectMode() {
		PresetManagerUseCases.get().startStoreSelectMode();
	}

	public void endStoreSelectMode() {
		PresetManagerUseCases.get().endStoreSelectMode();
	}

	public void startLoadToPartMode() {
		PresetManagerUseCases.get().startLoadToPartMode();
	}

	public void endLoadToPartMode() {
		PresetManagerUseCases.get().endLoadToPartMode();
	}

	@Override
	public NonLinearWorld getParent() {
		return (NonLinearWorld) super.getParent();
	}

	@Override
	public double getLevelOfDetailForFullVisibility() {
		return 1;
	}

	@Override
	public void calcPixRect(Position parentsReference, double currentZoom) {
		if (super.calcPixRectWithoutMargins(parentsReference, currentZoom))
			children.calcPixRect(this.getPixRect().getCenterPoint(), currentZoom);

		theHtmlUI.syncPosition();
	}

	@Override
	public void movePixRect(double x, double y) {
		super.movePixRect(x, y);
		theHtmlUI.syncPosition();
	}

	public void toggleHiddenBanks() {
		setVisible(!isIntendedVisible());
	}

	@Override
	public void doFirstLayoutPass(double levelOfDetail) {
	}

	@Override
	public void doSecondLayoutPass(double parentsWidthFromFirstPass, double parentsHeightFromFirstPass) {

	}

	public void handleUpKey() {
		// TODO
		// if (getMultiSelection() == null)
		{
			selectPreviousPreset();
		} // else {
			// getMultiSelection().handleUpKey();
			// }
	}

	private void handleDownKey() {
		// TODO
		// if (getMultiSelection() == null) {
		selectNextPreset();
		// } else {
		// getMultiSelection().handleDownKey();
		// }
	}

	@Override
	public Control onKey(final KeyDownEvent event) {

		var useCases = PresetManagerUseCases.get();
		int keyCode = event.getNativeEvent().getKeyCode();

		if (keyCode == com.google.gwt.event.dom.client.KeyCodes.KEY_RIGHT) {
			useCases.selectNextBank();
		} else if (keyCode == com.google.gwt.event.dom.client.KeyCodes.KEY_LEFT) {
			useCases.selectPreviousBank();
		} else if (keyCode == com.google.gwt.event.dom.client.KeyCodes.KEY_DOWN) {
			handleDownKey();
		} else if (keyCode == com.google.gwt.event.dom.client.KeyCodes.KEY_UP) {
			handleUpKey();
		} else if (keyCode == com.google.gwt.event.dom.client.KeyCodes.KEY_ENTER) {
			useCases.loadSelectedPreset();
		} else if (keyCode == com.google.gwt.event.dom.client.KeyCodes.KEY_P) {
			if (getNonMaps().getNonLinearWorld().isCtrlDown()) {
				toggleHiddenBanks();
			} else {
				PresetInfoDialog.toggle();
			}
		} else if (keyCode == com.google.gwt.event.dom.client.KeyCodes.KEY_DELETE) {
			var preset = PresetManagerPresenterProvider.get().getPresenter().selectedPreset;
			PresetContextMenu.deletePresetWithBankModal(preset);
		} else if (keyCode == com.google.gwt.event.dom.client.KeyCodes.KEY_Z
				&& NonMaps.get().getNonLinearWorld().isCtrlDown()) {
			NonMaps.get().getServerProxy().undo();
		} else if (keyCode == com.google.gwt.event.dom.client.KeyCodes.KEY_Y
				&& NonMaps.get().getNonLinearWorld().isCtrlDown()) {
			NonMaps.get().getServerProxy().redo();
		} else if (keyCode == com.google.gwt.event.dom.client.KeyCodes.KEY_F) {
			PresetSearchDialog.toggle();
		} else if (keyCode == com.google.gwt.event.dom.client.KeyCodes.KEY_U) {
			getNonMaps().getNonLinearWorld().getViewport().getOverlay().getUndoTree().toggle();
		} else if (keyCode == com.google.gwt.event.dom.client.KeyCodes.KEY_B) {
			BankInfoDialog.toggle();
		} else if (keyCode == com.google.gwt.event.dom.client.KeyCodes.KEY_I
				&& NonMaps.get().getNonLinearWorld().isCtrlDown() == false) {
			ParameterInfoDialog.toggle();
		} else if (keyCode == com.google.gwt.event.dom.client.KeyCodes.KEY_H
				&& NonMaps.get().getNonLinearWorld().isCtrlDown()) {
			Window.open("/online-help/index.html", "", "");
		} else if (keyCode == com.google.gwt.event.dom.client.KeyCodes.KEY_ESCAPE) {
			NonMaps.get().getNonLinearWorld().getViewport().getOverlay().removeExistingContextMenus();
			NonMaps.get().getNonLinearWorld().getViewport().getOverlay().collapseGlobalMenu();
			PresetManagerUseCases.get().finishMultipleSelection();
		} else if (keyCode == com.google.gwt.event.dom.client.KeyCodes.KEY_M
				&& NonMaps.get().getNonLinearWorld().isCtrlDown()) {
			Window.open("/NonMaps/MCView/index.html", "", "");
		} else if (keyCode == com.google.gwt.event.dom.client.KeyCodes.KEY_ONE
				|| keyCode == com.google.gwt.event.dom.client.KeyCodes.KEY_NUM_ONE) {
			EditBufferUseCases.get().selectVoiceGroup(VoiceGroup.I);
		} else if (keyCode == com.google.gwt.event.dom.client.KeyCodes.KEY_TWO
				|| keyCode == com.google.gwt.event.dom.client.KeyCodes.KEY_NUM_TWO) {
			EditBufferUseCases.get().selectVoiceGroup(VoiceGroup.II);
		} else {
			return null;
		}

		NonMaps.get().getNonLinearWorld().getViewport().getOverlay().refreshGlobalMenu();
		return this;
	}

	public void selectPreviousPreset() {
		PresetManagerUseCases.get().selectPreviousPreset();
	}

	public void selectNextPreset() {
		PresetManagerUseCases.get().selectNextPreset();
	}

	public void loadSelectedPresetPart() {
		PresetManagerUseCases.get().loadSelectedPresetPart();
	}

	public static int getSnapGridResolution() {
		return 15;
	}

	public boolean isChangingPresetWhileInDirectLoad() {
		// TODO
		// boolean directLoadActive =
		// PresetManagerPresenterProvider.get().getPresenter().loadModeButtonState;

		// if (!directLoadActive)
		// return false;

		// Preset loadedPreset = findLoadedPreset();
		// boolean isInitSound = loadedPreset == null;

		// if (isInitSound)
		// return false;

		// return findSelectedPreset() != loadedPreset;
		return false;
	}

};
