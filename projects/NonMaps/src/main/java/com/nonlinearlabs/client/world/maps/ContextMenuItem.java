package com.nonlinearlabs.client.world.maps;

import com.google.gwt.canvas.client.Canvas;
import com.google.gwt.canvas.dom.client.Context2d;
import com.google.gwt.canvas.dom.client.Context2d.TextAlign;
import com.google.gwt.canvas.dom.client.Context2d.TextBaseline;
import com.nonlinearlabs.client.Millimeter;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Dimension;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.RGBA;
import com.nonlinearlabs.client.world.Rect;

public class ContextMenuItem extends MapsControl {

	private String text = "";
	private final NonDimension dimension;

	public ContextMenuItem(ContextMenu parent, String text) {
		super(parent);
		this.text = text;

		Canvas canvas = Canvas.createIfSupported();
		Context2d context = canvas.getContext2d();

		double heightInPixels = Millimeter.toPixels(8);
		double fontHeightInPixels = heightInPixels / 2;

		context.setFont(fontHeightInPixels + "px 'SSP-LW25'");
		double width = context.measureText(text).getWidth();
		width /= getCurrentZoom();

		this.dimension = toNonDimension(new Dimension(heightInPixels, heightInPixels));
		this.dimension.setWidth(width + 10);
	}

	@Override
	public Control click(Position eventPoint) {
		getParent().fadeOut();
		return this;
	}

	@Override
	public Control mouseDown(Position eventPoint) {
		return this;
	}

	@Override
	public ContextMenu getParent() {
		return (ContextMenu) super.getParent();
	}

	@Override
	public double getLevelOfDetailForFullVisibility() {
		return 0;
	}

	@Override
	public void draw(Context2d ctx, int invalidationMask) {

		double fontHeightInPixels = toYPixels(getNonPosition().getHeight() / 2);

		Rect r = getPixRect();

		ctx.setStrokeStyle(new RGB(255, 255, 255).toString());
		ctx.setFillStyle(new RGBA(255, 255, 255, 0.8).toString());
		ctx.setLineWidth(2);
		ctx.fillRect(r.getLeft(), r.getTop(), r.getWidth(), r.getHeight());

		ctx.setTextAlign(TextAlign.LEFT);
		ctx.setFillStyle(getColorFont().toString());
		ctx.setTextBaseline(TextBaseline.MIDDLE);
		ctx.setFont(fontHeightInPixels + "px 'SSP-LW25'");

		ctx.fillText(text, r.getLeft() + toXPixels(2), r.getCenterPoint().getY());
		super.draw(ctx, invalidationMask);
	}

	@Override
	public RGB getColorFont() {
		return new RGB(0, 0, 0);
	}

	@Override
	public void doFirstLayoutPass(double levelOfDetail) {
		setNonSize(dimension);
	}

}
