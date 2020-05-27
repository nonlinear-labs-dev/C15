package com.nonlinearlabs.client.world.maps;

import java.util.Date;
import java.util.Iterator;
import java.util.LinkedList;

import com.google.gwt.canvas.client.Canvas;
import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.Checksum;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.overlay.DragProxy;
import com.nonlinearlabs.client.world.pointer.Gesture;
import com.nonlinearlabs.client.world.pointer.TouchPinch;

public class CachingMapsControl extends MapsLayout {

	private MapsControl child;
	private long hashOnDrawing = 0;
	private double zoomOnDrawing = 0.0;
	private Canvas bmp = Canvas.createIfSupported();
	private static LinkedList<CachingMapsControl> bgRedrawControls = new LinkedList<CachingMapsControl>();

	public CachingMapsControl(MapsLayout parent, MapsControl child) {
		super(parent);
		this.child = child;
		child.setParent(this);
	}

	public Control handleGesture(Gesture g) {
		return child.handleGesture(g);
	}

	public void gestureFocusLost() {
		child.gestureFocusLost();
	}

	public void gestureFocusCaptured() {
		child.gestureFocusCaptured();
	}

	@Override
	public void getStateHash(Checksum crc) {
		super.getStateHash(crc);
		child.getStateHash(crc);
	}

	@Override
	public void doFirstLayoutPass(double levelOfDetail) {
		child.doFirstLayoutPass(levelOfDetail);
		setNonSize(child.getNonPosition().getDimension());
	}

	@Override
	public double getMaxLevelOfDetail() {
		return child.getMaxLevelOfDetail();
	}

	@Override
	public void doSecondLayoutPass(double parentsWidthFromFirstPass, double parentsHeightFromFirstPass) {
		child.doSecondLayoutPass(parentsWidthFromFirstPass, parentsHeightFromFirstPass);
	}

	@Override
	public boolean moveTo(double x, double y) {
		child.moveTo(x, y);
		return getNonPosition().moveTo(x, y);
	}

	public boolean isTooLargeToCache() {
		Rect screenSize = NonMaps.theMaps.getNonLinearWorld().getViewport().getPixRect();
		Rect mySize = getPixRect();
		return (2 * mySize.getWidth() >= screenSize.getWidth()) || (2 * mySize.getHeight() >= screenSize.getHeight());
	}

	private boolean shouldCache() {
		return !isTooLargeToCache() && getBitmapCacheSetting();
	}

	private boolean getBitmapCacheSetting() {
		String e = NonMaps.theMaps.getNonLinearWorld().getSettings().get("BitmapCache", "on").toLowerCase();
		return e.equals("on");
	}

	private boolean isRedrawingInBackground = false;

	@Override
	public boolean isRectVisible(Rect r) {
		if (isRedrawingInBackground)
			return true;

		return super.isRectVisible(r);
	}

	private void redrawInBackground(long newHash, int invalidationMask) {
		Rect r = getPixRect();

		boolean shouldCache = shouldCache();

		int w = (int) (r.getWidth() + 1);
		int h = (int) (r.getHeight() + 1);

		if (!shouldCache) {
			w = 0;
			h = 0;
		}

		if (bmp.getCoordinateSpaceHeight() != h || bmp.getCoordinateSpaceWidth() != w) {
			bmp.setPixelSize(w, h);
			bmp.setCoordinateSpaceWidth(w);
			bmp.setCoordinateSpaceHeight(h);
		}

		if (shouldCache) {
			Context2d bgCtx = bmp.getContext2d();
			bgCtx.save();
			bgCtx.translate(-r.getLeft(), -r.getTop());
			isRedrawingInBackground = true;
			child.draw(bgCtx, invalidationMask);
			isRedrawingInBackground = false;
			bgCtx.restore();

			hashOnDrawing = newHash;
			zoomOnDrawing = getCurrentZoom();
		}
	}

	@Override
	public void draw(Context2d ctx, int invalidationMask) {
		if (shouldCache())
			drawFromCache(ctx, invalidationMask);
		else
			child.draw(ctx, invalidationMask);
	}

	protected void drawFromCache(Context2d ctx, int invalidationMask) {
		Rect r = getPixRect();

		if ((invalidationMask & INVALIDATION_FLAG_UI_CHANGED) != 0) {
			long newHash = getHash();
			boolean hashChanged = newHash != hashOnDrawing;
			boolean zoomChanged = getCurrentZoom() != zoomOnDrawing;

			if (hashChanged) {
				redrawInBackground(newHash, invalidationMask);
			} else if (zoomChanged) {
				scheduleBackgroundRefresh(newHash, invalidationMask);
			}
		}

		if (bmp.getCanvasElement().getWidth() > 0 && bmp.getCanvasElement().getHeight() > 0) // Fixes new bug that whole
																								// groups wouldnt be
																								// drawn sometimes:
																								// Failed to execute
																								// 'drawImage' on
																								// 'CanvasRenderingContext2D':
																								// The image argument is
																								// a canvas element with
																								// a width or height of
																								// 0.
			ctx.drawImage(bmp.getCanvasElement(), r.getLeft(), r.getTop(), r.getWidth(), r.getHeight());
	}

	private long bgRedrawHash = 0;
	private int bgRedrawinvalidationMask = 0;

	private void scheduleBackgroundRefresh(long newHash, int invalidationMask) {
		bgRedrawHash = newHash;
		bgRedrawinvalidationMask |= invalidationMask;
		bgRedrawControls.remove(this);
		bgRedrawControls.add(0, this);
	}

	public static void doSomeRedraws(int maxMS) {
		long start = new Date().getTime();

		Iterator<CachingMapsControl> it = bgRedrawControls.iterator();

		while (it.hasNext()) {
			CachingMapsControl c = it.next();
			c.redrawInBackground(c.bgRedrawHash, c.bgRedrawinvalidationMask);
			c.bgRedrawinvalidationMask = 0;
			c.invalidate(INVALIDATION_FLAG_BITMAP_CACHE_REFRESHED);
			it.remove();

			long now = new Date().getTime();
			if (now - start >= maxMS)
				break;
		}
	}

	private long getHash() {
		Checksum crc = new Checksum();
		getStateHash(crc);
		long newHash = crc.getHash();
		return newHash;
	}

	@Override
	public void calcPixRect(Position parentsReference, double zoom) {
		child.calcPixRect(parentsReference, zoom);
		setPixRect(child.getPixRect());
	}

	@Override
	public void movePixRect(double x, double y) {
		super.movePixRect(x, y);
		child.movePixRect(x, y);
	}

	public double getXMargin() {
		return child.getXMargin();
	}

	public double getYMargin() {
		return child.getYMargin();
	}

	public double getLeftMargin() {
		return child.getLeftMargin();
	}

	public double getRightMargin() {
		return child.getRightMargin();
	}

	public double getTopMargin() {
		return child.getTopMargin();
	}

	public double getBottomMargin() {
		return child.getBottomMargin();
	}

	public double getWidthMargin() {
		return child.getWidthMargin();
	}

	public double getHeightMargin() {
		return child.getHeightMargin();
	}

	public boolean setNonSize(double width, double height) {
		boolean ret = child.setNonSize(width, height);
		ret |= super.setNonSize(width, height);
		return ret;
	}

	public boolean setNonSize(NonDimension dim) {
		return setNonSize(dim.getWidth(), dim.getHeight());
	}

	public Control mouseDown(Position eventPoint) {
		return child.mouseDown(eventPoint);
	}

	public Control mouseDrag(Position oldPoint, Position newPoint, boolean fine) {
		return child.mouseDrag(oldPoint, newPoint, fine);
	}

	public Control mouseUp(Position eventPoint) {
		return child.mouseUp(eventPoint);
	}

	public Control click(Position eventPoint) {
		return child.click(eventPoint);
	}

	public Control longPress(Position eventPoint) {
		return child.longPress(eventPoint);
	}

	public Control longLeftPress(Position eventPoint) {
		return child.longLeftPress(eventPoint);
	}

	public Control longRightPress(Position eventPoint) {
		return child.longRightPress(eventPoint);
	}

	public Control pinch(Position eventPoint, double touchDist, TouchPinch pinch) {
		return child.pinch(eventPoint, touchDist, pinch);
	}

	public Control wheel(Position eventPoint, double amount, boolean fine) {
		return child.wheel(eventPoint, amount, fine);
	}

	public Control onContextMenu(Position pos) {
		return child.onContextMenu(pos);
	}

	public Control startDragging(Position pos) {
		return child.startDragging(pos);
	}

	public Control drag(Position pos, DragProxy dragProxy) {
		return child.drag(pos, dragProxy);
	}

	public void dragLeave() {
		child.dragLeave();
	}

	public Control drop(Position pos, DragProxy dragProxy) {
		return child.drop(pos, dragProxy);
	}

	public void onMouseLost() {
		child.onMouseLost();
	}

	public void onMouseCaptured() {
		child.onMouseCaptured();
	}

	public void onRemoved() {
		child.onRemoved();
	}

	@Override
	public Control doubleClick(Position pos) {
		return child.doubleClick(pos);
	}

	@Override
	public Control recurseChildren(ControlFinder handler) {
		return child.recurseChildren(handler);
	}

	public Control recurseChildren(Position pos, ControlFinder handler) {
		return child.recurseChildren(pos, handler);
	}
}
