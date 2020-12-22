package com.nonlinearlabs.client.world.maps.parameters;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.maps.MapsLayout;

class Line extends ZoomReactingControl {

	private double width;
	private double height;

	public Line(MapsLayout parent, double width, double height) {
		super(parent);
		this.width = width;
		this.height = height;
	}

	@Override
	protected double getBasicWidth() {
		return width;
	}

	@Override
	protected double getBasicHeight() {
		return height;
	}

	@Override
	protected double getMinHeight() {
		return 0;
	}

	@Override
	public void draw(Context2d ctx, Context2d overlay, Context2d menus, int invalidationMask) {
		Rect pixRect = getPixRect();

		ctx.setFillStyle(getColorObjectContour().toString());
		double insetX = 0;
		double insetY = 0;
		ctx.fillRect(pixRect.getLeft() + insetX, pixRect.getTop() + insetY, pixRect.getWidth() - 2 * insetX,
				pixRect.getHeight() - 2 * insetY);
		super.draw(ctx, overlay, menus, invalidationMask);
	}
}
