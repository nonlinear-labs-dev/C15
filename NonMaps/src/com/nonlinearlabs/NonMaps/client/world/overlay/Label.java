package com.nonlinearlabs.NonMaps.client.world.overlay;

import com.google.gwt.canvas.dom.client.Context2d;
import com.google.gwt.canvas.dom.client.Context2d.TextAlign;
import com.google.gwt.canvas.dom.client.Context2d.TextBaseline;
import com.nonlinearlabs.NonMaps.client.Millimeter;
import com.nonlinearlabs.NonMaps.client.contextStates.ClipContext;
import com.nonlinearlabs.NonMaps.client.world.Position;
import com.nonlinearlabs.NonMaps.client.world.RGB;
import com.nonlinearlabs.NonMaps.client.world.Rect;
import com.nonlinearlabs.NonMaps.client.world.TextCropper;

public abstract class Label extends OverlayControl {

	public Label(OverlayLayout parent) {
		super(parent);
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

		String text = TextCropper.crop(getDrawText(ctx), ctx, pixRect.getWidth());
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
			ctx.fillText(text, left.getX(), left.getY() + getVerticalFontDisplacement());
		} else {
			double x = left.getX();
			for (int i = 0; i < splits.length; i++) {
				ctx.setFillStyle(getColorFontForSplit(i).toString());
				ctx.fillText(splits[i], x, left.getY() + getVerticalFontDisplacement());
				x += ctx.measureText(splits[i]).getWidth();
			}
		}
	}

	protected RGB getColorFontForSplit(int i) {
		return getColorFont();
	}

	protected Rect getTextRect() {
		return getPixRect();
	}

	private void setFont(Context2d ctx, double fontHeightInPixels) {
		ctx.setFont(fontHeightInPixels + "px nonlinearfont");
	}

	protected TextAlign getAlignment() {
		return TextAlign.CENTER;
	}

	protected double getFontHeight(Rect pixRect) {
		return Millimeter.toPixels(4);
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

}
