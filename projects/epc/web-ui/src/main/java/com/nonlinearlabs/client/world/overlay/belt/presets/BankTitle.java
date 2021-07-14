package com.nonlinearlabs.client.world.overlay.belt.presets;

import com.google.gwt.canvas.dom.client.Context2d;
import com.google.gwt.canvas.dom.client.Context2d.TextAlign;
import com.google.gwt.canvas.dom.client.Context2d.TextBaseline;
import com.nonlinearlabs.client.Millimeter;
import com.nonlinearlabs.client.world.Gray;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.maps.presets.bank.Bank;
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
		Bank bank = getParent().getParent().getBankInCharge();

		if (bank != null) {
			return bank.getOrderNumber() + " - " + bank.getCurrentName();
		} else {
			return "---";
		}
	}

	@Override
	public void draw(Context2d ctx, int invalidationMask) {
		Rect pix = getPixRect();
		pix.fill(ctx, new Gray(51));
		super.draw(ctx, invalidationMask);

		if(getParent().getParent().getBankInCharge().isMidiBank())
			drawMidiSymbol(ctx);
	}

	private void drawMidiSymbol(Context2d ctx) {
		Rect pixRect = getPixRect();

		double fontHeightInUnits = getFontHeight(pixRect);

		TextAlign align = TextAlign.RIGHT;
		ctx.setTextAlign(align);
		ctx.setFillStyle(getColorFont().toString());
		ctx.setFont(fontHeightInUnits + "px " + getFontName());

		Position center = pixRect.getCenterPoint();
		center.setX(pixRect.getRight());
		
		ctx.setTextBaseline(TextBaseline.MIDDLE);
		double offset = Millimeter.toPixels(1);
		ctx.fillText("\uE0C1", center.getX() - offset, center.getY());
	}


	@Override
	public RGB getColorFont() {
		return new Gray(204);
	}
}
