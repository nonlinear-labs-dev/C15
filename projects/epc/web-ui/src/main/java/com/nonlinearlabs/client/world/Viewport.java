package com.nonlinearlabs.client.world;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.Millimeter;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.dataModel.setup.SetupModel;
import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.NonDimension;
import com.nonlinearlabs.client.world.overlay.Overlay;
import com.nonlinearlabs.client.world.pointer.Gesture;

public class Viewport extends MapsLayout {

	private Overlay overlay;

	Viewport(NonLinearWorld parent) {
		super(parent);

		SetupModel.get().localSettings.stripeBrightness.onChange(v -> {
			invalidate(INVALIDATION_FLAG_UI_CHANGED);
			return true;
		});
	}

	public void initViewport() {
		overlay = new Overlay(this);
	}

	public Overlay getOverlay() {
		return overlay;
	}

	@Override
	public NonLinearWorld getParent() {
		return (NonLinearWorld) super.getParent();
	}

	void setScreenDimension(int width, int height) {
		getPixRect().getDimension().set(width, height);
	}

	@Override
	public void calcPixRect(Position parentsReference, double currentZoom) {
	}

	@Override
	public void movePixRect(double x, double y) {
	}

	@Override
	public void draw(Context2d ctx, Context2d o, Context2d menus, int invalidationMask) {
		super.draw(ctx, o, menus, invalidationMask);
		overlay.draw(o, null, menus, invalidationMask);
	}

	@Override
	public Control recurseChildren(ControlFinder handler) {
		if (handler.onWayDownFound(this))
			return this;

		Control c = overlay.recurseChildren(handler);
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

			Control c = overlay.recurseChildren(pos, handler);
			if (c != null)
				return c;

			if (handler.onWayUpFound(this))
				return this;
		}
		return null;
	}

	@Override
	public Control recurseChildren(Rect r, ControlFinder handler) {
		if (getPixRect().intersects(r)) {
			if (handler.onWayDownFound(this))
				return this;

			Control c = overlay.recurseChildren(r, handler);
			if (c != null)
				return c;

			if (handler.onWayUpFound(this))
				return this;
		}
		return null;
	}

	@Override
	public Control handleGesture(Gesture g) {
		Control ret = overlay.handleGesture(g);

		if (ret != null)
			return ret;

		return super.handleGesture(g);
	}

	@Override
	public void doFirstLayoutPass(double levelOfDetail) {
		overlay.doLayout(0, 0, getPixRect().getWidth(), getPixRect().getHeight());
	}

	@Override
	public double getCurrentZoom() {
		return NonMaps.getPixelFactor() * getPixRect().getWidth() / getNonPosition().getWidth();
	}

	public void setCurrentZoom(double zoom) {
		double nonWidth = NonMaps.getPixelFactor() * getPixRect().getWidth() / zoom;
		getNonPosition().setDimension(nonWidth, nonWidth / NonMaps.theMaps.getScreenRatio());
		requestLayout();
	}

	void drawBackground(Context2d ctx) {
		getPixRect().fill(ctx, RGB.black());
		drawBackgroundPattern(ctx);
	}

	private void drawBackgroundPattern(Context2d ctx) {
		drawCobweb(ctx);
	}

	private void drawCobweb(Context2d ctx) {
		ctx.setLineWidth(1);

		Rect rect = getPixRect();
		Position center = getParent().getPixRect().getCenterPoint();
		center.moveBy(rect.getLeft(), rect.getTop());
		double width = rect.getWidth();
		double height = rect.getHeight();

		double radius = Math.max(width, height);
		double numStripes = 16;

		double arcInc = 2 * Math.PI / numStripes;
		double arc = -0.15 * 2 * Math.PI;

		double brightness = SetupModel.get().localSettings.stripeBrightness.toPercent();

		if (brightness > 0) {
			ctx.setStrokeStyle(new Gray((int) (255 * brightness / 100)).toString());

			ctx.beginPath();

			for (int i = 0; i < (int) numStripes; i++) {
				double x = width / 2 + Math.cos(arc) * radius;
				double y = height / 2 + Math.sin(arc) * radius;

				ctx.moveTo(x, y);
				ctx.lineTo(center.getX(), center.getY());
				arc += arcInc;
			}

			ctx.closePath();
			ctx.stroke();
		}
	}

	public void autoScroll(int x, int y) {
		double s = Millimeter.toPixels(5);
		NonDimension nonDim = toNonDimension(new Dimension(x * s, y * s));
		moveBy(nonDim);
		requestLayout();
	}

	public Rect getPixRectWithoutBelt() {
		Rect r = getPixRect().copy();
		Rect beltRect = getOverlay().getBelt().getPixRect();
		r.setHeight(beltRect.getTop() - r.getTop());
		return r;
	}
}
