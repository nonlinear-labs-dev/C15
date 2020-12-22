package com.nonlinearlabs.client.world.overlay;

import java.util.ArrayList;

import com.google.gwt.canvas.dom.client.Context2d;
import com.google.gwt.canvas.dom.client.Context2d.Composite;
import com.google.gwt.core.client.GWT;
import com.google.gwt.dom.client.Document;
import com.google.gwt.dom.client.Style.Unit;
import com.google.gwt.event.dom.client.DragEndEvent;
import com.google.gwt.event.dom.client.DragEvent;
import com.google.gwt.event.dom.client.DragLeaveEvent;
import com.google.gwt.event.dom.client.DragOverEvent;
import com.google.gwt.event.dom.client.DropEvent;
import com.google.gwt.user.client.ui.Anchor;
import com.google.gwt.user.client.ui.FocusWidget;
import com.google.gwt.user.client.ui.HTMLPanel;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.world.ChildrenOwner;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.ILayout;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.pointer.Gesture;

public abstract class OverlayLayout extends OverlayControl implements ILayout<OverlayControl> {

	FocusWidget htmlProxy;

	protected OverlayLayout(Control parent) {
		this(parent, false, false);
	}

	protected OverlayLayout(Control parent, boolean draggable, boolean droppable) {
		super(parent);

		if (draggable || droppable || parent instanceof Overlay) {
			var overlay = Document.get().getElementById("overlay-proxies");
			var overlayWidget = HTMLPanel.wrap(overlay);
			overlayWidget.add(htmlProxy = new Anchor());
			htmlProxy.getElement().addClassName("overlay-proxy");
			getNonMaps().initHandlers(htmlProxy);

			if (droppable) {
				htmlProxy.addDomHandler(e -> {
					e.preventDefault();
					e.stopPropagation();
					GWT.log("drop on overlay");
					drop(new Position(e.getNativeEvent()), null);
					dragLeave();
				}, DropEvent.getType());

				htmlProxy.addDomHandler(e -> {
					e.preventDefault();
					e.stopPropagation();
					drag(new Position(e.getNativeEvent()), null);
					GWT.log("drag over on overlay");
				}, DragOverEvent.getType());
			}

			if (draggable) {
				htmlProxy.addDomHandler(e -> {
					e.preventDefault();
					e.stopPropagation();
					drag(new Position(e.getNativeEvent()), null);
					GWT.log("drag on overlay");
				}, DragEvent.getType());

				htmlProxy.addDomHandler(e -> {
					e.preventDefault();
					e.stopPropagation();
					dragLeave();
					GWT.log("drag end overlay");
				}, DragEndEvent.getType());

				htmlProxy.addDomHandler(e -> {
					e.preventDefault();
					e.stopPropagation();
					dragLeave();
					GWT.log("drag leave overlay");
				}, DragLeaveEvent.getType());
			}
		}

	}

	@Override
	public void setPixRect(Rect rect) {
		super.setPixRect(rect);
		syncPosition();
	}

	@Override
	public void movePixRect(double x, double y) {
		super.movePixRect(x, y);
		syncPosition();
	}

	@Override
	public void onRemoved() {
		if (htmlProxy != null)
			htmlProxy.removeFromParent();
	}

	private void syncPosition() {
		if (htmlProxy == null)
			return;

		var s = htmlProxy.getElement().getStyle();
		s.setLeft(getPixRect().getLeft() / NonMaps.devicePixelRatio, Unit.PX);
		s.setTop(getPixRect().getTop() / NonMaps.devicePixelRatio, Unit.PX);
		s.setWidth(getPixRect().getWidth() / NonMaps.devicePixelRatio, Unit.PX);
		s.setHeight(getPixRect().getHeight() / NonMaps.devicePixelRatio, Unit.PX);
	}

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
	public void drawChildren(Context2d ctx, Context2d overlay, Context2d menus, int invalidationMask) {
		double op = getOpacity();

		if (op == 1.0) {
			children.draw(ctx, overlay, menus, invalidationMask);
		} else if (op != 0.0) {
			ctx.save();
			ctx.setGlobalCompositeOperation(Composite.SOURCE_OVER);
			ctx.setGlobalAlpha(op);
			children.draw(ctx, overlay, menus, invalidationMask);
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
	public void draw(Context2d ctx, Context2d overlay, Context2d menus, int invalidationMask) {
		drawChildren(ctx, overlay, menus, invalidationMask);
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
