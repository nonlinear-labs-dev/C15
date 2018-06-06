package com.nonlinearlabs.NonMaps.client.world.overlay.belt;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.NonMaps.client.Millimeter;
import com.nonlinearlabs.NonMaps.client.world.Control;
import com.nonlinearlabs.NonMaps.client.world.Gray;
import com.nonlinearlabs.NonMaps.client.world.Position;
import com.nonlinearlabs.NonMaps.client.world.RGB;
import com.nonlinearlabs.NonMaps.client.world.Rect;
import com.nonlinearlabs.NonMaps.client.world.overlay.OverlayLayout;
import com.nonlinearlabs.NonMaps.client.world.overlay.SVGImage;

abstract public class BeltButton extends SVGImage {

	protected Belt belt = null;

	BeltButton(OverlayLayout parent, Belt belt, String active, String inactive) {
		super(parent, active, inactive);
		this.belt = belt;
	}

	@Override
	public Control click(Position eventPoint) {
		return this;
	}

	@Override
	public Control onContextMenu(Position pos) {
		return this;
	}

	@Override
	public RGB getColorFont() {
		if (!isActive())
			return super.getColorFont().darker(20);

		return super.getColorFont();
	}

	public int getSelectedPhase() {
		return isActive() ? 0 : 1;
	}

	@Override
	public void doLayout(double x, double y, double w, double h) {
		super.doLayout(x, y, w, h);
	}

	@Override
	public void draw(Context2d ctx, int invalidationMask) {
		if (!isActive() || belt.isHidden()) {
			double corner = Millimeter.toPixels(1);

			Rect r = getPixRect().copy();
			r.setHeight(r.getHeight() + corner);

			ctx.beginPath();
			ctx.moveTo(r.getLeft(), r.getBottom());
			ctx.lineTo(r.getLeft(), r.getTop() + corner);
			ctx.arcTo(r.getLeft(), r.getTop(), r.getLeft() + corner, r.getTop(), corner);
			ctx.lineTo(r.getRight() - corner, r.getTop());
			ctx.arcTo(r.getRight(), r.getTop(), r.getRight(), r.getTop() + corner, corner);
			ctx.lineTo(r.getRight(), r.getBottom());
			ctx.closePath();
			ctx.setLineWidth(5);
			ctx.setStrokeStyle(new Gray(0).toString());
			ctx.stroke();

			ctx.setFillStyle(new Gray(38).toString());
			ctx.fill();

			ctx.setLineWidth(2);
			ctx.setStrokeStyle(new Gray(54).toString());
			ctx.stroke();
		}

		super.draw(ctx, invalidationMask);
	}

	public abstract boolean isActive();

	@Override
	protected double getPadding() {
		return Millimeter.toPixels(2);
	}
}