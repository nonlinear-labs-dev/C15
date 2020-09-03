package com.nonlinearlabs.client.world.overlay;

import java.util.ArrayList;

import com.google.gwt.canvas.dom.client.Context2d;
import com.google.gwt.canvas.dom.client.Context2d.Composite;
import com.nonlinearlabs.client.world.ChildrenOwner;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.ILayout;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.pointer.Gesture;

public abstract class OverlayLayout extends OverlayControl implements ILayout<OverlayControl> {

	protected OverlayLayout(Control parent) {
		super(parent);
	}

	// private Animator opacityAnimator;
	private double opacity = 1.0;
	private ChildrenOwner<OverlayControl> children = new ChildrenOwner<OverlayControl>();

	@Override
	public Control handleGesture(Gesture g) {
		if (getOpacity() == 0.0)
			return null;

		Control ret = children.handleGesture(g);

		if (ret != null)
			return ret;

		return super.handleGesture(g);
	}

	@Override
	public ArrayList<OverlayControl> getChildren() {
		return children.getChildren();
	}

	@Override
	public <T extends OverlayControl> T addChild(T child) {
		return children.addChild(child);
	}

	@Override
	public <T extends OverlayControl> T prependChild(T child) {
		return children.prependChild(child);
	}

	@Override
	public <T extends OverlayControl> T insertChild(int idx, T child) {
		return children.insertChild(idx, child);
	}

	public void replaceChild(OverlayControl oldControl, OverlayControl newControl) {
		children.replaceChild(oldControl, newControl);
	}

	public void removeChild(OverlayControl control) {
		children.removeChild(control);
	}

	public void removeAll() {
		children.removeAll();
	}

	public void fadeIn() {
		opacity = 1.0;
		invalidate(INVALIDATION_FLAG_ANIMATION_PROGRESS);
	}

	public void fadeOut() {
		opacity = 0.0;
		invalidate(INVALIDATION_FLAG_ANIMATION_PROGRESS);
	}

	public double getOpacity() {
		return opacity;
	}

	@Override
	public void drawChildren(Context2d ctx, Context2d overlay, int invalidationMask) {
		double op = getOpacity();

		if (op == 1.0) {
			children.draw(ctx, overlay, invalidationMask);
		} else if (op != 0.0) {
			ctx.save();
			ctx.setGlobalCompositeOperation(Composite.SOURCE_OVER);
			ctx.setGlobalAlpha(op);
			children.draw(ctx, overlay, invalidationMask);
			ctx.restore();
		}
	}

	@Override
	public Control recurseChildren(ControlFinder handler) {
		if (handler.onWayDownFound(this))
			return this;

		Control c = children.recurseChildren(handler);
		if (c != null)
			return c;

		if (handler.onWayUpFound(this))
			return this;

		return null;
	}

	@Override
	public Control recurseChildren(Position pos, ControlFinder handler) {
		if (getPixRect().contains(pos)) {
			if (handler.onWayDownFound(this))
				return this;

			Control c = children.recurseChildren(pos, handler);
			if (c != null)
				return c;

			if (handler.onWayUpFound(this))
				return this;
		}
		return null;
	}

	public Control recurseChildren(Rect r, ControlFinder handler) {
		if (getPixRect().intersects(r)) {
			if (handler.onWayDownFound(this))
				return this;

			Control c = children.recurseChildren(r, handler);
			if (c != null)
				return c;

			if (handler.onWayUpFound(this))
				return this;
		}
		return null;
	}

	@Override
	public void draw(Context2d ctx, Context2d overlay, int invalidationMask) {
		drawChildren(ctx, overlay, invalidationMask);
	}

	@Override
	public void calcPixRect(Position parentsReference, double currentZoom) {
		super.calcPixRect(parentsReference, currentZoom);
		children.calcPixRect(getPixRect().getPosition(), currentZoom);
	}

	@Override
	public boolean isVisible() {
		return super.isVisible() && getOpacity() > 0.0;
	}
}
