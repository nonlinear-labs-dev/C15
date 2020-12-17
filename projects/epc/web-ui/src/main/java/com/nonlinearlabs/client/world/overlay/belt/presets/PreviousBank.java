package com.nonlinearlabs.client.world.overlay.belt.presets;

import com.nonlinearlabs.client.presenters.PresetManagerPresenterProvider;
import com.nonlinearlabs.client.useCases.PresetManagerUseCases;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;

class PreviousBank extends PrevNextBank {

	PreviousBank(BankHeader parent) {
		super(parent, "Bank_Left_Enabled.svg", "Bank_Left_Active.svg", "Bank_Left_Disabled.svg");
	}

	@Override
	public Control click(Position eventPoint) {
		PresetManagerUseCases.get().selectPreviousBank();
		return this;
	}

	@Override
	public int getSelectedPhase() {
		if (!PresetManagerPresenterProvider.get().getValue().canSelectPrevBank)
			return drawStates.disabled.ordinal();

		return super.getSelectedPhase();
	}
}
