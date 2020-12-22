package com.nonlinearlabs.client.world;

import java.util.ArrayList;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.Checksum;
import com.nonlinearlabs.client.world.Control.ControlFinder;
import com.nonlinearlabs.client.world.pointer.Gesture;
import com.nonlinearlabs.client.world.pointer.PointerState;

public class ChildrenOwner<TChild extends Control> {
	private final ArrayList<TChild> m_children = new ArrayList<TChild>();

	public ArrayList<TChild> getChildren() {
		return m_children;
	}

	public <T extends TChild> T addChild(T child) {
		m_children.add(child);
		initChild(child);
		return child;
	}

	public void initChild(Control child) {
		if (!child.isInitialized())
			child.init();
	}

	public <T extends TChild> T prependChild(T child) {
		m_children.add(0, child);
		initChild(child);
		return child;
	}

	public <T extends TChild> T insertChild(int idx, T child) {
		m_children.add(idx, child);
		initChild(child);
		return child;
	}

	public void getStateHash(Checksum crc) {
		crc.eat(hashCode());

		for (TChild c : getChildren())
			c.getStateHash(crc);
	}

	public void removeChild(TChild control) {
		m_children.remove(control);
		control.onRemoved();
	}

	public void removeAll() {
		while (!m_children.isEmpty())
			removeChild(m_children.get(0));
	}

	public void draw(Context2d ctx, Context2d overlay, Context2d menus, int invalidationMask) {
		for (TChild c : getChildren()) {
			if (c.isVisible()) {
				c.draw(ctx, overlay, menus, invalidationMask | c.resetPendingInvalidations());
				// c.getPixRect().stroke(ctx, 1, RGB.red()); // debug feature
			} else {
				c.addPendingInvalidations(invalidationMask);
			}
		}
	}

	public void calcPixRect(Position parentsReference, double currentZoom) {
		for (TChild c : getChildren())
			c.calcPixRect(parentsReference, currentZoom);
	}

	public void movePixRect(double x, double y) {
		for (TChild c : getChildren())
			c.movePixRect(x, y);
	}

	public Control recurseChildren(ControlFinder handler) {
		int numControls = getChildren().size();
		for (int i = numControls - 1; i >= 0; i--) {
			Control c = getChildren().get(i);

			Control found = c.recurseChildren(handler);

			if (found != null)
				return found;
		}

		return null;
	}

	public Control recurseChildren(Position pos, ControlFinder handler) {
		int numControls = getChildren().size();
		for (int i = numControls - 1; i >= 0; i--) {
			Control c = getChildren().get(i);
			if (c.isVisible()) {
				Control found = c.recurseChildren(pos, handler);

				if (found != null)
					return found;
			}
		}

		return null;
	}

	public Control recurseChildren(Rect r, ControlFinder handler) {
		int numControls = getChildren().size();
		for (int i = numControls - 1; i >= 0; i--) {
			Control c = getChildren().get(i);

			Control found = c.recurseChildren(r, handler);

			if (found != null)
				return found;
		}

		return null;
	}

	public void forceVisibility(boolean b) {
		for (Control c : getChildren()) {
			c.forceVisibility(b);
		}
	}

	public Control handleGesture(Gesture g) {

		boolean hasPosition = g instanceof Gesture.HasPosition;
		int numControls = getChildren().size();
		Position mousePos = PointerState.get().getPosition();

		for (int i = numControls - 1; i >= 0; i--) {
			Control c = getChildren().get(i);

			boolean positionMatches = !hasPosition || c.getPixRect().contains(mousePos);

			if (positionMatches) {
				Control ret = c.handleGesture(g);
				if (ret != null)
					return ret;
			}
		}

		return null;
	}

	public void replaceChild(TChild oldControl, TChild newControl) {
		if (oldControl != newControl) {
			int idx = m_children.indexOf(oldControl);
			if (idx >= 0)
				m_children.set(idx, newControl);
			oldControl.onRemoved();
		}
	}

}
