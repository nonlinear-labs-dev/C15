package com.nonlinearlabs.NonMaps.client.world.overlay.belt.presets;

import com.google.gwt.xml.client.Node;
import com.nonlinearlabs.NonMaps.client.world.overlay.OverlayLayout;

public class BankControlArea extends OverlayLayout {

	private BankControl bank;

	public BankControlArea(OverlayLayout parent) {
		super(parent);
		addChild(bank = new BankControl(this));
		bank.setHeaderTitleFontHeightInMM(5);
	}

	@Override
	public void doLayout(double x, double y, double w, double h) {
		final double bankMargin = getButtonDimension() / 8;
		bank.doLayout(0, bankMargin, w, h - 2 * bankMargin);
		super.doLayout(x, y, bank.getRelativePosition().getWidth(), h);
	}

	public BankControl getBankControl() {
		return bank;
	}

	public void update(Node presetManagerNode) {
		bank.update(presetManagerNode);
	}

	public double getHorizontalCenterLinePosition() {
		return bank.getHorizontalCenterLinePosition() + getRelativePosition().getTop();
	}

	public void renameCurrentPreset() {
		bank.renameCurrentPreset();
	}
}
