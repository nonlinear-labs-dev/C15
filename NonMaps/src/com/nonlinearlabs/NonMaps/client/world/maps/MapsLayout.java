package com.nonlinearlabs.NonMaps.client.world.maps;

import java.util.ArrayList;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.NonMaps.client.Checksum;
import com.nonlinearlabs.NonMaps.client.world.ChildrenOwner;
import com.nonlinearlabs.NonMaps.client.world.Control;
import com.nonlinearlabs.NonMaps.client.world.ILayout;
import com.nonlinearlabs.NonMaps.client.world.Position;
import com.nonlinearlabs.NonMaps.client.world.RGB;
import com.nonlinearlabs.NonMaps.client.world.Rect;
import com.nonlinearlabs.NonMaps.client.world.pointer.Gesture;

public abstract class MapsLayout extends MapsControl implements ILayout<MapsControl> {
	static boolean debugRects = false;

	public MapsLayout(MapsControl parent) {
		super(parent);
	}

	protected ChildrenOwner<MapsControl> children = new ChildrenOwner<MapsControl>();

	@Override
	public Control handleGesture(Gesture g) {
		if (!isVisible())
			return null;

		Control ret = children.handleGesture(g);

		if (ret != null)
			return ret;

		return super.handleGesture(g);
	}

	@Override
	public ArrayList<MapsControl> getChildren() {
		return children.getChildren();
	}

	@Override
	public void getStateHash(Checksum crc) {
		super.getStateHash(crc);
		children.getStateHash(crc);
	}

	@Override
	public <T extends MapsControl> T addChild(T child) {
		return children.addChild(child);
	}

	@Override
	public <T extends MapsControl> T prependChild(T child) {
		return children.prependChild(child);
	}

	@Override
	public <T extends MapsControl> T insertChild(int idx, T child) {
		return children.insertChild(idx, child);
	}

	public void removeChild(MapsControl control) {
		children.removeChild(control);
	}

	public void replaceChild(MapsControl oldControl, MapsControl newControl) {
		children.replaceChild(oldControl, newControl);
	}

	public void removeAll() {
		children.removeAll();
	}

	@Override
	public void drawChildren(Context2d ctx, int invalidationMask) {
		children.draw(ctx, invalidationMask);

		if (debugRects) {
			for (MapsControl c : getChildren()) {
				if (c instanceof MapsLayout) {

				} else {
					c.getPixRect().stroke(ctx, 1, new RGB(255, 0, 0));
				}
			}
		}
	}

	@Override
	public void forceVisibility(boolean b) {
		super.forceVisibility(b);
		children.forceVisibility(b);
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
	public void draw(Context2d ctx, int invalidationMask) {
		drawChildren(ctx, invalidationMask);

		if (debugRects)
			getPixRect().stroke(ctx, 1, new RGB(255, 0, 0));
	}

	@Override
	public void calcPixRect(Position parentsReference, double currentZoom) {
		if (calcPixRectWithoutMargins(parentsReference, currentZoom))
			children.calcPixRect(this.getPixRect().getPosition(), currentZoom);
	}

	@Override
	public void movePixRect(double x, double y) {
		if (!getPixRect().isEmpty(2)) {
			super.movePixRect(x, y);
			children.movePixRect(x, y);
		}
	}

	@Override
	public double getMaxLevelOfDetail() {
		double ret = 0;

		for (MapsControl c : getChildren()) {
			ret = Math.max(ret, c.getMaxLevelOfDetail());
		}
		return ret;
	}

	public boolean calcPixRectWithoutMargins(Position parentsReference, double zoom) {
		Rect r = getPixRect();
		NonRect nonPosition = getNonPosition();

		double xu = nonPosition.getPosition().getX();
		double yu = nonPosition.getPosition().getY();
		double wu = nonPosition.getDimension().getWidth();
		double hu = nonPosition.getDimension().getHeight();

		double x = xu * zoom + parentsReference.getX();
		double y = yu * zoom + parentsReference.getY();
		double w = wu * zoom;
		double h = hu * zoom;
		r.set(x, y, Math.max(0, w), Math.max(0, h));

		return !getPixRect().isEmpty(2);
	}

	public double getPadding() {
		return 0;
	}

	public void moveBy(NonDimension dist) {
		getNonPosition().moveBy(dist);

	}
}
