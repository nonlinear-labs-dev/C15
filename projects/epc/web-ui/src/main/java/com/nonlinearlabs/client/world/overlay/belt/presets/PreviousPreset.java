package com.nonlinearlabs.client.world.overlay.belt.presets;

import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.presenters.BankPresenterProviders;
import com.nonlinearlabs.client.presenters.PresetManagerPresenterProvider;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.maps.presets.PresetManager;
import com.nonlinearlabs.client.world.overlay.SVGImage;

class PreviousPreset extends SVGImage {

	PreviousPreset(PrevNextPresetButtons parent) {
		super(parent, "Preset_Up_Enabled.svg", "Preset_Up_Active.svg", "Preset_Up_Disabled.svg");
	}

	@Override
	public Control click(Position eventPoint) {
		doPresetSelection();
		return this;
	}

	@Override
	public Control longLeftPress(Position eventPoint) {
		ButtonRepeat.get().start(new Runnable() {

			@Override
			public void run() {
				doPresetSelection();
			}
		});
		return super.longLeftPress(eventPoint);
	}

	@Override
	public Control mouseDown(Position eventPoint) {
		return this;
	}

	@Override
	public void onMouseLost() {
		ButtonRepeat.get().cancel();
		super.onMouseLost();
	}

	private void doPresetSelection() {
		PresetManager pm = NonMaps.theMaps.getNonLinearWorld().getPresetManager();
		pm.selectPreviousPreset();
	}

	@Override
	public int getSelectedPhase() {
		var pm = PresetManagerPresenterProvider.get().getValue();
		var b = BankPresenterProviders.get().getPresenter(pm.selectedBank);

		if (!b.canSelectPrevPreset)
			return drawStates.disabled.ordinal();

		return super.getSelectedPhase();
	}
}
