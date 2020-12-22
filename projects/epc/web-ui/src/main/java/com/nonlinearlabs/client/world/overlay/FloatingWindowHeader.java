package com.nonlinearlabs.client.world.overlay;

import com.google.gwt.canvas.dom.client.Context2d;
import com.google.gwt.canvas.dom.client.Context2d.LineCap;
import com.google.gwt.canvas.dom.client.Context2d.LineJoin;
import com.nonlinearlabs.client.Millimeter;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Gray;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.Rect;

public class FloatingWindowHeader extends OverlayLayout {

	protected Label header;
	protected Label x;

	public FloatingWindowHeader(final FloatingWindow parent) {
		super(parent);

		addChild(header = new Label(this) {

			@Override
			public String getDrawText(Context2d ctx) {
				return parent.getTitle();
			}
		});

		addChild(x = new Label(this) {

			@Override
			public String getDrawText(Context2d ctx) {
				return "";
			}

			@Override
			public void draw(Context2d ctx, Context2d overlay, Context2d menus, int invalidationMask) {
				drawX(ctx);
			}

			private void drawX(Context2d ctx) {
				Rect r = getPixRect().getReducedBy(Millimeter.toPixels(4));

				boolean mouseDown = isCaptureControl();

				RGB fillColor = mouseDown ? new Gray(133) : new Gray(66);
				RGB strokeColor = mouseDown ? new Gray(222) : new Gray(154);

				r.drawRoundedRect(ctx, Rect.ROUNDING_ALL, Millimeter.toPixels(1), Millimeter.toPixels(0.25), fillColor,
						strokeColor);

				r = r.getReducedBy(Millimeter.toPixels(4));

				ctx.beginPath();
				ctx.moveTo(r.getLeft(), r.getTop());
				ctx.lineTo(r.getRight(), r.getBottom());
				ctx.moveTo(r.getRight(), r.getTop());
				ctx.lineTo(r.getLeft(), r.getBottom());

				ctx.setStrokeStyle(strokeColor.toString());
				ctx.setLineWidth(Millimeter.toPixels(0.5));
				ctx.setLineCap(LineCap.SQUARE);
				ctx.setLineJoin(LineJoin.MITER);

				ctx.stroke();
			}

			@Override
			public Control mouseDown(Position eventPoint) {
				invalidate(INVALIDATION_FLAG_UI_CHANGED);
				return this;
			}

			@Override
			public void onMouseLost() {
				invalidate(INVALIDATION_FLAG_UI_CHANGED);
			}

			@Override
			public Control mouseUp(Position eventPoint) {
				invalidate(INVALIDATION_FLAG_UI_CHANGED);
				return this;
			}

			@Override
			public void dragLeave() {
				invalidate(INVALIDATION_FLAG_UI_CHANGED);
			}

			@Override
			public Control click(Position eventPoint) {
				parent.toggle();
				return this;
			}
		});
	}

	@Override
	public FloatingWindow getParent() {
		return (FloatingWindow) super.getParent();
	}

	@Override
	public void doLayout(double x, double y, double w, double h) {
		super.doLayout(x, y, w, h);
		double buttonDim = Millimeter.toPixels(10);
		this.x.doLayout(w - buttonDim, (h - buttonDim) / 2, buttonDim, buttonDim);
		this.header.doLayout(buttonDim, 0, w - 2 * buttonDim, h);
	}

	protected void toggle() {
		getParent().toggle();
	}

	@Override
	public Control mouseDown(Position eventPoint) {
		return this;
	}

	@Override
	public Control mouseUp(Position eventPoint) {
		return this;
	}

	@Override
	public Control startDragging(Position pos) {
		return this;
	}

	@Override
	public Control mouseDrag(Position oldPoint, Position newPoint, boolean fine) {
		getParent().getRelativePosition().moveBy(newPoint.getX() - oldPoint.getX(), newPoint.getY() - oldPoint.getY());
		preventWindowOutsideOfCanvasTopAndBottom();
		preventWindowOutsideOfLeftAndRight();
		requestLayout();
		return this;
	}

	public void preventWindowOutsideOfCanvasTopAndBottom() {
		double minY = 0;
		double maxY = NonMaps.theMaps.getCanvas().getCoordinateSpaceHeight() - getPixRect().getHeight();

		Rect relPosition = getParent().getRelativePosition();

		if (relPosition.getTop() < minY)
			relPosition.moveTo(relPosition.getLeft(), minY);

		if (relPosition.getTop() > maxY)
			relPosition.moveTo(relPosition.getLeft(), maxY);
	}

	public void preventWindowOutsideOfLeftAndRight() {
		double minX = 0;
		double maxX = NonMaps.theMaps.getCanvas().getCoordinateSpaceWidth() - getPixRect().getWidth();
		Rect relPosition = getParent().getRelativePosition();
		double left = relPosition.getLeft();

		if (left < (minX - (relPosition.getWidth() / 2)))
			relPosition.moveTo(minX - relPosition.getWidth() / 2, relPosition.getTop());

		if ((left - (relPosition.getWidth() / 2)) > maxX)
			relPosition.moveTo(maxX + relPosition.getWidth() / 2, relPosition.getTop());
	}

	@Override
	public Control click(Position eventPoint) {
		return this;
	}

}