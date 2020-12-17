package com.nonlinearlabs.client.world.overlay.belt.presets;

import com.nonlinearlabs.client.presenters.PresetManagerPresenterProvider;
import com.nonlinearlabs.client.useCases.PresetManagerUseCases;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;

class NextBank extends PrevNextBank {

	NextBank(BankHeader parent) {
		super(parent, "Bank_Right_Enabled.svg", "Bank_Right_Active.svg", "Bank_Right_Disabled.svg");
	}

	@Override
	public Control click(Position eventPoint) {
		PresetManagerUseCases.get().selectNextBank();
		return this;
	}

	@Override
	public int getSelectedPhase() {
		if (!PresetManagerPresenterProvider.get().getValue().canSelectNextBank)
			return drawStates.disabled.ordinal();

		return super.getSelectedPhase();
	}
}
