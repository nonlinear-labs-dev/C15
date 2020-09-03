package com.nonlinearlabs.client.world.overlay.belt.parameters;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.Millimeter;
import com.nonlinearlabs.client.presenters.EditBufferPresenterProvider;
import com.nonlinearlabs.client.presenters.ParameterPresenter;
import com.nonlinearlabs.client.world.Gray;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.overlay.OverlayControl;

public class DottedLine extends OverlayControl {

	public DottedLine(BeltParameterLayout parent) {
		super(parent);
	}

	@Override
	public void draw(Context2d ctx, Context2d overlay, int invalidationMask) {
		ParameterPresenter p = EditBufferPresenterProvider.getPresenter().selectedParameter;

		if (p.modulation.isModulated) {
			final double dim = Millimeter.toPixels(0.5);
			final Rect r = getPixRect();
			final Position center = r.getCenterPoint();
			final RGB color = new Gray(128);
			ctx.setFillStyle(color.toString());

			final double stepSize = 2 * dim;
			final double left = r.getLeft() + stepSize;
			final double right = r.getRight() - stepSize;

			for (double x = left; x < right; x += stepSize)
				ctx.fillRect(x, center.getY() - dim / 2, dim, dim);
		}
	}
}
