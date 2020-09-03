package com.nonlinearlabs.client.world.overlay.belt.parameters.recall;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.Millimeter;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Dimension;
import com.nonlinearlabs.client.world.Gray;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.overlay.OverlayLayout;
import com.nonlinearlabs.client.world.overlay.belt.parameters.BeltParameterLayout;

public abstract class RecallArea extends OverlayLayout {

	protected RecallValue value;

	protected RecallArea(BeltParameterLayout parent) {
		super(parent);
	}

	@Override
	public void draw(Context2d ctx, Context2d overlay, int invalidationMask) {
		if (isChanged()) {
			super.draw(ctx, overlay, invalidationMask);
			drawTriangle(ctx, new Gray(77));
		}
	}

	private void drawTriangle(Context2d ctx, RGB color) {
		double mm3 = Millimeter.toPixels(3);
		Dimension offset = new Dimension(getPixRect().getWidth() - mm3, 0);
		Rect movedToRight = getPixRect().getMovedBy(offset);
		movedToRight = movedToRight.getReducedBy(movedToRight.getHeight() / 3);

		ctx.beginPath();
		ctx.setLineWidth(1);
		ctx.moveTo(movedToRight.getLeft(), movedToRight.getTop());
		ctx.lineTo(movedToRight.getLeft() + mm3, movedToRight.getCenterPoint().getY());
		ctx.lineTo(movedToRight.getLeft(), movedToRight.getBottom());
		ctx.lineTo(movedToRight.getLeft(), movedToRight.getTop());
		ctx.setFillStyle(color.toString());
		ctx.setStrokeStyle(color.brighter(15).toString());
		ctx.closePath();
		ctx.fill();
		ctx.stroke();
	}

	@Override
	public void doLayout(double x, double y, double w, double h) {
		double hParts = h / 8;
		y = y + hParts * 2;
		h = hParts * 4;
		double mm3 = Millimeter.toPixels(3);
		super.doLayout(x, y, w, h);
		value.doLayout(0, 0, w - mm3, h);
	}

	@Override
	public Control click(Position eventPoint) {
		if (isChanged()) {
			resetValue();
			return this;
		}
		return super.click(eventPoint);
	}

	abstract public void setVisibleForMode(BeltParameterLayout.Mode mode);

	abstract public boolean isChanged();

	abstract public void resetValue();
}
