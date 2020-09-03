package com.nonlinearlabs.client.world.overlay.belt.parameters;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.Millimeter;
import com.nonlinearlabs.client.dataModel.editBuffer.ParameterId;
import com.nonlinearlabs.client.presenters.EditBufferPresenterProvider;
import com.nonlinearlabs.client.presenters.ParameterPresenter;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.RGBA;
import com.nonlinearlabs.client.world.overlay.OverlayControl;
import com.nonlinearlabs.client.world.overlay.OverlayLayout;

public abstract class MCSomething extends OverlayLayout {

	protected OverlayControl middle;

	public MCSomething(Control parent) {
		super(parent);
	}

	public OverlayControl getMiddle() {
		return middle;
	}

	@Override
	public BeltParameterLayout getParent() {
		return (BeltParameterLayout) super.getParent();
	}

	@Override
	public void draw(Context2d ctx, Context2d overlay, int invalidationMask) {

		boolean drawArrows = middle.getPixRect().getWidth() >= Millimeter.toPixels(50);

		if (middle.getPixRect().getWidth() > Millimeter.toPixels(25))
			middle.getPixRect().drawValueEditSliderBackgound(ctx, drawArrows, getColorFont());

		super.draw(ctx, overlay, invalidationMask);

		if (shouldDrawShadow()) {
			getPixRect().fill(ctx, new RGBA(getColorModuleBackground(), 0.5));
		}
	}

	protected boolean shouldDrawShadow() {
		ParameterPresenter p = EditBufferPresenterProvider.getPresenter().selectedParameter;
		return !p.modulation.isModulated;
	}

	@Override
	public void doLayout(double x, double y, double w, double h) {
		double maxWidth = Millimeter.toPixels(60);
		double xMargin = 0;
		double yMargin = h / 8;

		if (w > maxWidth) {
			xMargin = (w - maxWidth) / 2;
			w = maxWidth;
		}

		super.doLayout(x + xMargin, y + yMargin, w, h - 2 * yMargin);
		middle.doLayout(0, 0, getRelativePosition().getWidth(), getRelativePosition().getHeight());
	}

	@Override
	public Control wheel(Position eventPoint, double amount, boolean fine) {
		ParameterPresenter p = EditBufferPresenterProvider.getPresenter().selectedParameter;

		if (p.modulation.isModulateable) {
			if (amount > 0)
				inc(p.id, fine);
			else if (amount < 0)
				dec(p.id, fine);
		}

		return this;
	}

	protected abstract void dec(ParameterId id, boolean fine);

	protected abstract void inc(ParameterId id, boolean fine);

}