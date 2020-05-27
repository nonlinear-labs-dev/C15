package com.nonlinearlabs.client.world;

import com.google.gwt.canvas.dom.client.Context2d;
import com.google.gwt.event.dom.client.KeyDownEvent;
import com.nonlinearlabs.client.Checksum;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.world.overlay.DragProxy;
import com.nonlinearlabs.client.world.pointer.Gesture;
import com.nonlinearlabs.client.world.pointer.PointerState;
import com.nonlinearlabs.client.world.pointer.TouchPinch;

public abstract class Control {
	private Control parent;
	private Rect pixPosition = new Rect();
	private boolean forceVisibility = false;
	private boolean visible = true;
	private int pendingInvalidationMask = 0;
	private boolean initialized = false;

	public static int INVALIDATION_FLAG_UI_CHANGED = 1 << 0;
	public static int INVALIDATION_FLAG_SCROLLED = 1 << 1;
	public static int INVALIDATION_FLAG_ZOOMED = 1 << 2;
	public static int INVALIDATION_FLAG_BITMAP_CACHE_REFRESHED = 1 << 3;
	public static int INVALIDATION_FLAG_VISIBILITY_CHANGED = 1 << 4;
	public static int INVALIDATION_FLAG_ANIMATION_PROGRESS = 1 << 5;

	public Control(Control parent) {
		super();
		setParent(parent);
	}

	public void init() {
		initialized = true;
	}

	public boolean isInitialized() {
		return initialized;
	}

	public void getStateHash(Checksum crc) {
		crc.eat(hashCode());
	}

	public void setParent(Control parent) {
		this.parent = parent;
	}

	public void draw(Context2d ctx, int invalidationMask) {
	}

	public void drawPost(Context2d ctx, int invalidationMask) {
	}

	public void calcPixRect(Position parentsReference, double currentZoom) {
	}

	public void setPixRect(Rect rect) {
		pixPosition.set(rect);
		pixPosition.getDimension().makeNonNegative();
	}

	public void movePixRect(double x, double y) {
		pixPosition.moveBy(x, y);
	}

	public boolean isRectVisible(Rect r) {
		return visible && getParent().isRectVisible(r);
	}

	public boolean isVisible() {
		if (visible && getPixRect().getHeight() > 0 && getPixRect().getWidth() > 0) {
			return forceVisibility || isRectVisible(getPixRect());
		}

		return false;
	}

	public boolean isIntendedVisible() {
		return visible || forceVisibility;
	}

	public boolean isVisibilityForced() {
		return forceVisibility;
	}

	public void setVisible(boolean v) {
		boolean wasVisible = isVisible();
		visible = v;

		if (wasVisible != isVisible())
			invalidate(INVALIDATION_FLAG_VISIBILITY_CHANGED);
	}

	public void invalidate(int flags) {
		parent.invalidate(flags);
	}

	public Rect getPixRect() {
		return pixPosition;
	}

	public Control getParent() {
		return parent;
	}

	public void requestLayout() {
		parent.requestLayout();
	}

	public NonMaps getNonMaps() {
		return NonMaps.theMaps;
	}

	public boolean wantsMouseUpAnimation() {
		return true;
	}

	public class ControlFinder {
		public boolean onWayDownFound(Control ctrl) {
			return false;
		}

		public boolean onWayUpFound(Control ctrl) {
			return false;
		}
	}

	public Control recurseChildren(ControlFinder handler) {
		if (handler.onWayDownFound(this) || handler.onWayUpFound(this))
			return this;

		return null;
	}

	public Control recurseChildren(Position pos, ControlFinder handler) {
		if (getPixRect().contains(pos))
			if (handler.onWayDownFound(this) || handler.onWayUpFound(this))
				return this;

		return null;
	}

	public Control recurseChildren(Rect r, ControlFinder handler) {
		if (getPixRect().intersects(r))
			if (handler.onWayDownFound(this) || handler.onWayUpFound(this))
				return this;

		return null;
	}

	public RGB getColorFont() {
		return parent.getColorFont();
	}

	public RGB getColorFontValueDisplay() {
		return parent.getColorFontValueDisplay();
	}

	public RGB getColorFontValueDisplaySelected() {
		return parent.getColorFontValueDisplaySelected();
	}

	public RGB getHeaderColorFont() {
		return parent.getHeaderColorFont();
	}

	public RGB getColorSliderHighlight() {
		return parent.getColorSliderHighlight();
	}

	public RGB getColorSliderBackground() {
		return parent.getColorSliderBackground();
	}

	public RGB getColorModuleBackground() {
		return parent.getColorModuleBackground();
	}

	public RGB getColorModuleHeaderBackground() {
		return parent.getColorModuleHeaderBackground();
	}

	public RGB getColorObjectBackgroundSelected() {
		return parent.getColorObjectBackgroundSelected();
	}

	public RGB getColorObjectContourSelected() {
		return parent.getColorObjectContourSelected();
	}

	public RGB getColorObjectContour() {
		return parent.getColorObjectContour();
	}

	public RGB getColorIndicator() {
		return parent.getColorIndicator();
	}

	public void forceVisibility(boolean b) {
		forceVisibility = b;
	}

	public Control mouseDown(Position eventPoint) {
		return null;
	}

	public Control mouseDrag(Position oldPoint, Position newPoint, boolean fine) {
		return null;
	}

	public Control mouseUp(Position eventPoint) {
		return null;
	}

	public Control click(Position eventPoint) {
		return null;
	}

	public Control longLeftPress(Position eventPoint) {
		return longPress(eventPoint);
	}

	public Control longRightPress(Position eventPoint) {
		return longPress(eventPoint);
	}

	public Control longPress(Position eventPoint) {
		return null;
	}

	public Control pinch(Position eventPoint, double touchDist, TouchPinch pinch) {
		return null;
	}

	public Control wheel(Position eventPoint, double amount, boolean fine) {
		return null;
	}

	public Control onContextMenu(Position pos) {
		return null;
	}

	public Control startDragging(Position pos) {
		return null;
	}

	public Control drag(Position pos, DragProxy dragProxy) {
		return null;
	}

	public Control drag(Rect rect, DragProxy dragProxy) {
		return null;
	}

	public void dragLeave() {
	}

	public Control onKey(KeyDownEvent event) {
		return null;
	}

	public Control drop(Position pos, DragProxy dragProxy) {
		return null;
	}

	public void onMouseLost() {
	}

	public void onMouseCaptured() {
	}

	public void onRemoved() {
	}

	public Control doubleClick(Position pos) {
		return null;
	}

	public Control handleGesture(Gesture g) {
		if (isVisible()) {
			return g.applyTo(this);
		}
		return null;
	}

	public void gestureFocusLost() {
		onMouseLost();
	}

	public void gestureFocusCaptured() {
		onMouseCaptured();
	}

	public boolean isCaptureControl() {
		return PointerState.get().isCurrentReceiver(this);
	}

	public boolean isDraggingControl() {
		return getNonMaps().getNonLinearWorld().getViewport().getOverlay().isCurrentlyDragging(this)
				|| getParent().isDraggingControl();
	}

	public void addPendingInvalidations(int mask) {
		pendingInvalidationMask |= mask;
	}

	public int resetPendingInvalidations() {
		int ret = pendingInvalidationMask;
		pendingInvalidationMask = 0;
		return ret;
	}

	public void beingDragged(double xDiff, double yDiff) {
	}

	public int getDepth() {
		return getParent().getDepth() + 1;
	}

	public void beingDropped() {
	}

}
