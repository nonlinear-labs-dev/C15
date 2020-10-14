package com.nonlinearlabs.client.world.overlay.belt.presets;

import com.nonlinearlabs.client.world.overlay.OverlayLayout;

public class CombinedBankContextMenu extends BankContextMenu {

	public CombinedBankContextMenu(OverlayLayout parent, final String bank) {
		super(parent, bank);
	}

	@Override
	protected boolean hasCollapse() {
		return false;
	}

	@Override
	protected boolean hasPaste() {
		return false;
	}

	@Override
	protected boolean hasBankCreationRights() {
		return true;
	}
}
