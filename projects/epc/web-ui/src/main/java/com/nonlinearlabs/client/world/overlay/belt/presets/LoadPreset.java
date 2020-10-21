package com.nonlinearlabs.client.world.overlay.belt.presets;

import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.dataModel.setup.SetupModel;
import com.nonlinearlabs.client.presenters.BankPresenterProviders;
import com.nonlinearlabs.client.presenters.EditBufferPresenterProvider;
import com.nonlinearlabs.client.presenters.PresetManagerPresenterProvider;
import com.nonlinearlabs.client.useCases.PresetManagerUseCases;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.overlay.OverlayLayout;
import com.nonlinearlabs.client.world.overlay.SVGImage;

class LoadPreset extends SVGImage {

	LoadPreset(OverlayLayout parent) {
		super(parent, "Load_Enabled.svg", "Load_Active.svg", "Load_Disabled.svg");
	}

	public boolean isSelectedBankEmpty() {
		var pm = PresetManagerPresenterProvider.get().getPresenter();
		var bank = BankPresenterProviders.get().getPresenter(pm.selectedBank);
		return bank.presets.isEmpty();
	}

	@Override
	public int getSelectedPhase() {
		if (isSelectedBankEmpty()) {
			return drawStates.disabled.ordinal();
		} else if (!isEnabled()) {
			return drawStates.disabled.ordinal();
		} else if (PresetManagerPresenterProvider.get().getPresenter().inStoreSelectMode) {
			return drawStates.disabled.ordinal();
		} else if (isCaptureControl()) {
			return drawStates.active.ordinal();
		} else if (NonMaps.theMaps.getNonLinearWorld().getPresetManager().isChangingPresetWhileInDirectLoad()) {
			return drawStates.disabled.ordinal();
		} else {
			return drawStates.normal.ordinal();
		}
	};

	@Override
	public Control click(Position eventPoint) {
		var pm = PresetManagerPresenterProvider.get().getPresenter();
		if (pm.inStoreSelectMode)
			return this;

		if (isEnabled() && pm.inLoadToPartMode) {
			loadPart();
			return this;
		}

		if (isEnabled()) {
			load();
			return this;
		}
		return super.click(eventPoint);
	}

	boolean isEnabled() {
		boolean isDL = SetupModel.get().systemSettings.directLoad.getBool();

		if (!isSelectedPresetLoaded() && !isDL)
			return true;

		return EditBufferPresenterProvider.getPresenter().isAnyParameterChanged;
	}

	protected boolean isSelectedPresetLoaded() {
		boolean isLoadToPartActive = PresetManagerUseCases.get().isInLoadToPartMode();

		if (isLoadToPartActive) {
			return false;
		}

		var pm = PresetManagerPresenterProvider.get().getPresenter();
		var eb = EditBufferPresenterProvider.getPresenter();
		return pm.selectedPreset == eb.loadedPresetUUID;
	}

	public void load() {
		PresetManagerUseCases.get().loadSelectedPreset();
	}

	public void loadPart() {
		PresetManagerUseCases.get().loadSelectedPresetPart();
	}
}
