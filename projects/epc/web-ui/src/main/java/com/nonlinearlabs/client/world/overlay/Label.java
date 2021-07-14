package com.nonlinearlabs.client.world.overlay;

import com.google.gwt.canvas.dom.client.Context2d;
import com.google.gwt.canvas.dom.client.Context2d.TextAlign;
import com.google.gwt.canvas.dom.client.Context2d.TextBaseline;
import com.nonlinearlabs.client.Millimeter;
import com.nonlinearlabs.client.contextStates.ClipContext;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.TextCropper;

public abstract class Label extends OverlayControl {

	protected double fontHeightInMM = 0;
	RGB m_linecolor;

	protected RGB getLineColor() {
		return m_linecolor;
	}

	public void setFontHeightInMM(double fontHeightInMM) {
		this.fontHeightInMM = fontHeightInMM;
	}

	public Label(OverlayLayout parent) {
		super(parent);
		this.fontHeightInMM = 4;
	}

	public abstract String getDrawText(Context2d ctx);

	@Override
	public void draw(Context2d ctx, int invalidationMask) {
		try (ClipContext clipper = new ClipContext(ctx, this)) {
			drawClipped(ctx);
		}
	}

	protected void drawClipped(Context2d ctx) {
		Rect pixRect = getTextRect();
		double fontHeightInPixels = getFontHeight(pixRect);
		ctx.setTextAlign(TextAlign.LEFT);
		ctx.setFillStyle(getColorFont().toString());
		setFont(ctx, fontHeightInPixels);

		String text = getDrawText(ctx);
		text = crop(ctx, pixRect, text);
		Position left = pixRect.getCenterPoint();

		switch (getAlignment()) {
		case LEFT:
			left.setX(pixRect.getLeft());
			break;

		case CENTER:
			left.setX(pixRect.getLeft() + (pixRect.getWidth() - ctx.measureText(text).getWidth()) / 2);
			break;

		case RIGHT:
			left.setX(pixRect.getRight() - ctx.measureText(text).getWidth());
			break;

		default:
			break;
		}

		ctx.setTextBaseline(TextBaseline.MIDDLE);

		String splits[] = text.split("\t");

		if (splits.length == 1) {
			if (getLineColor() != null)
				ctx.setFillStyle(getLineColor().toString());

			drawText(ctx, text, left);
		} else {
			double x = left.getX();
			for (int i = 0; i < splits.length; i++) {
				String split = splits[i];

				double y = left.getY() + getVerticalFontDisplacement();
				drawSplit(i, ctx, split, x, y);
				x += ctx.measureText(split).getWidth();
			}
		}
	}

	protected void drawText(Context2d ctx, String text, Position left) {
		ctx.fillText(text, left.getX(), left.getY() + getVerticalFontDisplacement());
	}

	protected String crop(Context2d ctx, Rect pixRect, String text) {
		return TextCropper.crop(text, ctx, pixRect.getWidth());
	}

	public void drawSplit(int idx, Context2d ctx, String split, double x, double y) {
		RGB color = getColorFontForSplit(idx);
		ctx.setFillStyle(color.toString());
		drawText(ctx, split, new Position(x, y));
	}

	protected RGB getColorFontForSplit(int i) {
		return getColorFont();
	}

	protected Rect getTextRect() {
		return getPixRect();
	}

	protected void setFont(Context2d ctx, double fontHeightInPixels) {
		ctx.setFont(fontHeightInPixels + "px '" + getFontName() + "'");
	}

	public String getFontName() {
		return "SSP-LW25";
	}

	protected TextAlign getAlignment() {
		return TextAlign.CENTER;
	}

	protected double getFontHeight(Rect pixRect) {
		return Millimeter.toPixels(fontHeightInMM);
	}

	protected double getVerticalFontDisplacement() {
		return 0;
	}

	protected String chooseFittingString(Context2d ctx, String[] str) {
		Rect r = getTextRect();
		for (String text : str)
			if (ctx.measureText(text).getWidth() < r.getWidth() * 0.75)
				return text;

		return str[str.length - 1];
	}

	public void setFontColor(RGB rgb) {
		m_linecolor = rgb;
	}

}
