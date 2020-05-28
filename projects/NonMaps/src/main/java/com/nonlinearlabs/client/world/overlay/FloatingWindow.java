package com.nonlinearlabs.client.world.overlay;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.Millimeter;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Gray;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.pointer.Gesture;
import com.nonlinearlabs.client.world.pointer.Gesture.HasPosition;

public abstract class FloatingWindow extends OverlayLayout {

	private boolean shown = false;

	public FloatingWindow(Overlay parent) {
		super(parent);
	}

	@Override
	public Overlay getParent() {
		return (Overlay) super.getParent();
	}

	public boolean isShown() {
		return shown;
	}

	public boolean isHidden() {
		return !isShown();
	}

	public void toggle() {
		shown = !shown;

		if (shown) {
			onShow();
			getParent().pushToTop(this);
		}

		requestLayout();
	}

	public void onShow() {
	}

	@Override
	public void draw(Context2d ctx, int invalidationMask) {
		getPixRect().drawRoundedArea(ctx, Millimeter.toPixels(1), Millimeter.toPixels(1.5), null, RGB.black());

		getPixRect().drawRoundedArea(ctx, Millimeter.toPixels(1), Millimeter.toPixels(0.25),
				RGB.floatingWindowHeaderBackground(), Gray.floatingWindowHeaderBorder());
		super.draw(ctx, invalidationMask);
	}

	public abstract void doLayout(double x, double y);

	public abstract String getTitle();

	@Override
	public Control mouseDown(Position eventPoint) {
		return this;
	}

	@Override
	public Control mouseUp(Position eventPoint) {
		return this;
	}

	@Override
	public Control click(Position eventPoint) {
		return this;
	}

	@Override
	public Control longLeftPress(Position eventPoint) {
		return this;
	}

	@Override
	public Control longRightPress(Position eventPoint) {
		return this;
	}

	@Override
	public Control longPress(Position eventPoint) {
		return this;
	}

	@Override
	public Control doubleClick(Position pos) {
		return this;
	}

	@Override
	public Control wheel(Position eventPoint, double amount, boolean fine) {
		return this;
	}

	@Override
	public Control handleGesture(Gesture g) {
		if (g instanceof HasPosition) {
			HasPosition p = (HasPosition) g;
			if (getPixRect().contains(p.getPosition()))
				getParent().pushToTop(this);
		}

		return super.handleGesture(g);
	}

	public String getMenuTitle() {
		return getTitle();
	}

}
