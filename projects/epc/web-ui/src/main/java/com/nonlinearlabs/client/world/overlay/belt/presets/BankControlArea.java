package com.nonlinearlabs.client.world.overlay.belt.presets;

import com.nonlinearlabs.client.Millimeter;
import com.nonlinearlabs.client.world.overlay.OverlayLayout;

public class BankControlArea extends OverlayLayout {

	private BankControl bank;

	public BankControlArea(OverlayLayout parent) {
		super(parent);
		addChild(bank = new BankControl(this));
	}

	@Override
	public void doLayout(double x, double y, double w, double h) {
		final double xBankMargin = 0;
		final double yBankMargin = Millimeter.toPixels(2.5);
		bank.doLayout(xBankMargin, yBankMargin, w - 2 * xBankMargin, h - 2 * yBankMargin);
		super.doLayout(x, y, bank.getRelativePosition().getWidth(), h);
	}

	public BankControl getBankControl() {
		return bank;
	}

	public double getHorizontalCenterLinePosition() {
		return bank.getHorizontalCenterLinePosition() + getRelativePosition().getTop();
	}
}
