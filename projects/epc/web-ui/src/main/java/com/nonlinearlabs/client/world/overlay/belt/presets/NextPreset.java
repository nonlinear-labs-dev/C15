package com.nonlinearlabs.client.world.overlay.belt.presets;

import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.presenters.BankPresenterProviders;
import com.nonlinearlabs.client.presenters.PresetManagerPresenterProvider;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.maps.presets.PresetManager;
import com.nonlinearlabs.client.world.overlay.SVGImage;

class NextPreset extends SVGImage {

	NextPreset(PrevNextPresetButtons parent) {
		super(parent, "Preset_Down_Enabled.svg", "Preset_Down_Active.svg", "Preset_Down_Disabled.svg");
	}

	@Override
	public Control click(Position eventPoint) {
		doPresetSelection();
		return this;
	}

	private void doPresetSelection() {
		PresetManager pm = NonMaps.theMaps.getNonLinearWorld().getPresetManager();
		pm.selectNextPreset();
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

	@Override
	public int getSelectedPhase() {
		var pm = PresetManagerPresenterProvider.get().getValue();
		var bank = BankPresenterProviders.get().getPresenter(pm.selectedBank);

		if (bank.presets.isEmpty())
			return drawStates.disabled.ordinal();

		if (!bank.canSelectNextPreset)
			return drawStates.disabled.ordinal();

		return super.getSelectedPhase();
	}
}
