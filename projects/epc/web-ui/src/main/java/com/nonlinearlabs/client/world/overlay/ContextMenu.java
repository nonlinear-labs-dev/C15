package com.nonlinearlabs.client.world.overlay;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.Animator;
import com.nonlinearlabs.client.Animator.DoubleClientData.Client;
import com.nonlinearlabs.client.Millimeter;
import com.nonlinearlabs.client.contextStates.AlphaContextState;
import com.nonlinearlabs.client.contextStates.ContextState;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Gray;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.Rect;

public class ContextMenu extends OverlayLayout {

	private double opacity = 1.0;
	private Animator fadeOut = null;

	public ContextMenu(OverlayLayout parent) {
		super(parent);

		if (htmlProxy != null) {
			htmlProxy.getElement().addClassName("context-menu");
		}
	}

	@Override
	public OverlayLayout getParent() {
		return (OverlayLayout) super.getParent();
	}

	@Override
	public Control mouseDrag(Position oldPoint, Position newPoint, boolean fine) {
		return this;
	}

	@Override
	public Control mouseUp(Position eventPoint) {
		return this;
	}

	@Override
	public Control mouseDown(Position eventPoint) {
		fadeOut();
		return super.mouseDown(eventPoint);
	}

	@Override
	public void draw(Context2d ctx, Context2d overlay, Context2d menus, int invalidationMask) {
		try (ContextState fade = new AlphaContextState(menus, opacity)) {
			Rect r = getPixRect();
			r.drawRoundedArea(menus, Millimeter.toPixels(1), Millimeter.toPixels(0.7), RGB.black(), RGB.black());
			r.drawRoundedArea(menus, Millimeter.toPixels(1), Millimeter.toPixels(0.25), new Gray(77), new Gray(128));
			super.draw(menus, null, null, invalidationMask);
		}
	}

	private void hide() {
		getParent().removeChild(this);
		getParent().requestLayout();
	}

	public void fadeOut() {
		if (fadeOut == null) {
			fadeOut = new Animator(500);
			fadeOut.addSubAnimation(opacity, 0.0, new Client() {

				@Override
				public void animate(double v) {
					opacity = v;

					if (opacity <= 0)
						hide();

					invalidate(INVALIDATION_FLAG_ANIMATION_PROGRESS);
				}
			});
		}
	}

	public double getDesiredWidth() {
		double maxWidth = 0;

		for (OverlayControl c : getChildren()) {
			if (c instanceof ContextMenuItem) {
				ContextMenuItem i = (ContextMenuItem) c;
				maxWidth = Math.max(maxWidth, i.getDesiredWidth());
			}
		}

		return maxWidth + 2 * getXMargin();
	}

	public double getDesiredHeight() {
		double maxHeight = 0;

		for (OverlayControl c : getChildren()) {
			if (c instanceof ContextMenuItem) {
				ContextMenuItem i = (ContextMenuItem) c;
				maxHeight += i.getDesiredHeight();
			}
		}

		return maxHeight;
	}

	@Override
	public void doLayout(double x, double y, double w, double h) {
		super.doLayout(x, y, w, h);

		double y1 = 0;
		double x1 = getXMargin();

		for (OverlayControl c : getChildren()) {
			ContextMenuItem i = (ContextMenuItem) c;
			double h1 = i.getDesiredHeight();
			c.doLayout(x1, y1, w - 2 * x1, h1);
			y1 += h1;
		}
	}

	private double getXMargin() {
		return Millimeter.toPixels(2.5);
	}
}
