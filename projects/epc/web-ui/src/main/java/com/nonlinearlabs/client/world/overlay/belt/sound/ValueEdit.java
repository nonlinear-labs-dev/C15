package com.nonlinearlabs.client.world.overlay.belt.sound;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.Millimeter;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.dataModel.editBuffer.ParameterId;
import com.nonlinearlabs.client.presenters.ParameterPresenter;
import com.nonlinearlabs.client.presenters.ParameterPresenterProviders;
import com.nonlinearlabs.client.useCases.EditBufferUseCases;
import com.nonlinearlabs.client.useCases.IncrementalChanger;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.overlay.Label;
import com.nonlinearlabs.client.world.overlay.OverlayControl;
import com.nonlinearlabs.client.world.overlay.OverlayLayout;

public class ValueEdit extends OverlayLayout {

	private IncrementalChanger changer;
	private ParameterId parameter;

	private class ValueLabel extends Label {

		private ParameterPresenter presenter;

		ValueLabel(OverlayLayout parent, ParameterId id, boolean registerForCurrent) {
			super(parent);

			if (registerForCurrent) {
				ParameterPresenterProviders.get().registerForCurrentVoiceGroup(id.getNumber(), p -> {
					presenter = p;
					invalidate(INVALIDATION_FLAG_UI_CHANGED);
					return true;
				});
			} else {
				ParameterPresenterProviders.get().register(id, p -> {
					presenter = p;
					invalidate(INVALIDATION_FLAG_UI_CHANGED);
					return true;
				});
			}
		}

		@Override
		public String getDrawText(Context2d ctx) {
			return presenter.displayValues[0];
		}

		@Override
		protected Rect getTextRect() {
			Rect r = super.getTextRect().copy();
			r.reduceWidthBy(Millimeter.toPixels(5));
			return r;
		}

		@Override
		public Control doubleClick(Position pos) {
			EditBufferUseCases.get().setToDefault(parameter);
			return this;
		}
	}

	private class ValueArrow extends OverlayControl {

		private boolean isLeft;

		ValueArrow(OverlayLayout parent, boolean left) {
			super(parent);
			isLeft = left;
		}

		@Override
		public Control click(Position eventPoint) {
			boolean fine = NonMaps.get().getNonLinearWorld().isShiftDown();

			if (isLeft) {
				EditBufferUseCases.get().decParameter(parameter, fine);
			} else {
				EditBufferUseCases.get().incParameter(parameter, fine);
			}
			return this;
		}

		@Override
		public Control doubleClick(Position pos) {
			return click(null);
		}

		@Override
		public void draw(Context2d ctx, int invalidationMask) {
			return;
		}
	}

	private ValueLabel value = null;
	private ValueArrow left = null;
	private ValueArrow right = null;

	public ValueEdit(OverlayLayout parent, ParameterId param, boolean registerForCurrent) {
		super(parent);
		this.parameter = param;

		addChild(value = new ValueLabel(this, param, registerForCurrent));
		addChild(left = new ValueArrow(this, true));
		addChild(right = new ValueArrow(this, false));

		if (registerForCurrent) {
			ParameterPresenterProviders.get().registerForCurrentVoiceGroup(param.getNumber(), p -> {
				parameter = p.id;
				invalidate(INVALIDATION_FLAG_UI_CHANGED);
				return true;
			});
		} else {
			ParameterPresenterProviders.get().register(param, p -> {
				parameter = p.id;
				invalidate(INVALIDATION_FLAG_UI_CHANGED);
				return true;
			});
		}
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
			EditBufferUseCases.get().decParameter(parameter, fine);

		return this;
	}

	@Override
	public void draw(Context2d ctx, int invalidationMask) {
		boolean withArrows = getPixRect().getWidth() >= Millimeter.toPixels(35);
		getPixRect().drawValueEditSliderBackgound(ctx, withArrows, getColorFont());

		super.draw(ctx, invalidationMask);
		value.draw(ctx, invalidationMask);
	}

	@Override
	public void doLayout(double x, double y, double w, double h) {
		super.doLayout(x, y, w, h);
		value.doLayout(0, 0, w, h);

		double arrowWidth = w / 5;
		left.doLayout(0, 0, arrowWidth, h);
		right.doLayout(0 + w - arrowWidth, 0, arrowWidth, h);
	}

}