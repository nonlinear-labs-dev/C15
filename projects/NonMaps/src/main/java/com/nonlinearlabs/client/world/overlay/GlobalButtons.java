package com.nonlinearlabs.client.world.overlay;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.Millimeter;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.overlay.belt.Belt;
import com.nonlinearlabs.client.world.overlay.belt.IActivatable;
import com.nonlinearlabs.client.world.overlay.belt.ParameterBeltButton;
import com.nonlinearlabs.client.world.overlay.belt.PresetBeltButton;
import com.nonlinearlabs.client.world.overlay.belt.SoundBeltButton;

class GlobalButtons extends OverlayLayout {

	private SoundBeltButton sound;
	private ParameterBeltButton parameter;
	private PresetBeltButton preset;
	private Belt belt;

	GlobalButtons(final Overlay overlay, Belt belt) {
		super(overlay);

		this.belt = belt;

		sound = addChild(new SoundBeltButton(this, belt));
		parameter = addChild(new ParameterBeltButton(this, belt));
		preset = addChild(new PresetBeltButton(this, belt));
	}

	@Override
	public void doLayout(double x, double y, double w, double h) {
		super.doLayout(x, y, w, h);

		double width = w / 3;

		sound.doLayout(0 * width, 0, width, h);
		preset.doLayout(1 * width, 0, width, h);
		parameter.doLayout(2 * width, 0, width, h);
	}

	@Override
	public void calcPixRect(Position parentsReference, double currentZoom) {
		super.calcPixRect(parentsReference, currentZoom);
	}

	@Override
	public void draw(Context2d ctx, int invalidationMask) {
	}

	@Override
	public Control doubleClick() {
		return this;
	}

	public double getLeftOfActiveButton() {
		for (OverlayControl c : getChildren()) {
			IActivatable b = (IActivatable) c;
			if (b.isActive())
				return c.getPixRect().getLeft();
		}

		return getPixRect().getLeft();
	}

	public double getRightOfActiveButton() {
		for (OverlayControl c : getChildren()) {
			IActivatable b = (IActivatable) c;
			if (b.isActive())
				return c.getPixRect().getRight();
		}

		return getPixRect().getRight();
	}

	public void drawInactiveButton(Context2d ctx, int invalidationMask) {
		for (OverlayControl c : getChildren()) {
			IActivatable b = (IActivatable) c;
			if (!b.isActive() || belt.isHidden())
				c.draw(ctx, invalidationMask);
		}
	}

	public void drawActiveButton(Context2d ctx, int invalidationMask) {
		if (!belt.isHidden()) {
			for (OverlayControl c : getChildren()) {
				IActivatable b = (IActivatable) c;
				if (b.isActive())
					c.draw(ctx, invalidationMask);
			}
		}
	}

	@Override
	public Control wheel(Position eventPoint, double amount, boolean fine) {
		return this;
	}

	public void doLayout(double right, double bottom) {
		double xMargin = Millimeter.toPixels(6.5);
		double yMargin = Millimeter.toPixels(2.5);
		double w = sound.getSelectedImage().getImgWidth() + preset.getSelectedImage().getImgWidth()
				+ parameter.getSelectedImage().getImgWidth();
		w += 4 * xMargin;
		double h = sound.getSelectedImage().getImgHeight() + 2 * yMargin;
		doLayout(right - w, bottom - h, w, h);
	}

}
