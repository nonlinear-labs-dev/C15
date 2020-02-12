package com.nonlinearlabs.client.world.overlay.belt.sound;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.Millimeter;
import com.nonlinearlabs.client.dataModel.editBuffer.ParameterId;
import com.nonlinearlabs.client.presenters.ParameterPresenter;
import com.nonlinearlabs.client.presenters.ParameterPresenterProviders;
import com.nonlinearlabs.client.useCases.EditBufferUseCases;
import com.nonlinearlabs.client.useCases.IncrementalChanger;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.overlay.Label;
import com.nonlinearlabs.client.world.overlay.OverlayLayout;

class ValueEdit extends Label {

	private IncrementalChanger changer;
	private ParameterId parameter;
	private ParameterPresenter presenter;

	ValueEdit(OverlayLayout parent, ParameterId param) {
		super(parent);
		this.parameter = param;

		ParameterPresenterProviders.get().register(parameter, p -> {
			presenter = p;
			invalidate(INVALIDATION_FLAG_UI_CHANGED);
			return true;
		});
	}

	@Override
	public String getDrawText(Context2d ctx) {
		return presenter.displayValues[0];
	}

	@Override
	public Control click(Position eventPoint) {
		Rect leftRect = getPixRect().copy();
		leftRect.setRight(getPixRect().getLeft() + getPixRect().getWidth() / 2);
		Rect rightRect = getPixRect().copy();
		rightRect.setLeft(getPixRect().getRight() - getPixRect().getWidth() / 2);
		if (leftRect.contains(eventPoint)) {
			EditBufferUseCases.get().decParameter(parameter, false);
			return this;
		} else if (rightRect.contains(eventPoint)) {
			EditBufferUseCases.get().incParameter(parameter, false);
			return this;
		}
		return super.click(eventPoint);
	}

	@Override
	public Control mouseDown(Position eventPoint) {
		changer = EditBufferUseCases.get().startEditParameterValue(parameter, Millimeter.toPixels(100));
		return this;
	}

	@Override
	public Control mouseUp(Position eventPoint) {
		changer = null;
		return this;
	}

	@Override
	public Control doubleClick() {
		EditBufferUseCases.get().setToDefault(parameter.getNumber());
		return this;
	}

	@Override
	public Control mouseDrag(Position oldPoint, Position newPoint, boolean fine) {
		double xPix = newPoint.getX() - oldPoint.getX();
		double yPix = oldPoint.getY() - newPoint.getY();
		double pix = xPix;

		if (Math.abs(yPix) > Math.abs(xPix))
			pix = yPix;

		if (changer != null)
			changer.changeBy(fine, pix);

		return this;
	}

	@Override
	public Control wheel(Position eventPoint, double amount, boolean fine) {
		if (amount > 0)
			EditBufferUseCases.get().incParameter(parameter, fine);
		else if (amount < 0)
			EditBufferUseCases.get().incParameter(parameter, fine);

		return this;
	}

	@Override
	public void draw(Context2d ctx, int invalidationMask) {
		boolean withArrows = getPixRect().getWidth() >= Millimeter.toPixels(50);
		getPixRect().drawValueEditSliderBackgound(ctx, withArrows, getColorFont());
		super.draw(ctx, invalidationMask);
	}

	@Override
	protected Rect getTextRect() {
		Rect r = super.getTextRect().copy();
		r.reduceWidthBy(Millimeter.toPixels(5));
		return r;
	}
}