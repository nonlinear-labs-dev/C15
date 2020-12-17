package com.nonlinearlabs.client.world.overlay.belt.presets;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.world.Gray;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.overlay.Label;

class BankTitle extends Label {

	BankTitle(BankHeader parent) {
		super(parent);
		super.setFontHeightInMM(5.1);
	}

	@Override
	public BankHeader getParent() {
		return (BankHeader) super.getParent();
	}

	@Override
	public String getDrawText(Context2d ctx) {
		return getParent().getParent().getBankPresenter().name;
	}

	@Override
	public void draw(Context2d ctx, Context2d overlay, int invalidationMask) {
		Rect pix = getPixRect();
		pix.fill(ctx, new Gray(51));
		super.draw(ctx, overlay, invalidationMask);
	}

	@Override
	public RGB getColorFont() {
		return new Gray(204);
	}
}
