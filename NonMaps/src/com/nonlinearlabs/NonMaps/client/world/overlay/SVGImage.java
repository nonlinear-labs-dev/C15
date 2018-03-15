package com.nonlinearlabs.NonMaps.client.world.overlay;

import java.util.ArrayList;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.NonMaps.client.world.Control;
import com.nonlinearlabs.NonMaps.client.world.Position;
import com.nonlinearlabs.NonMaps.client.world.Rect;

public abstract class SVGImage extends OverlayControl {

	public enum drawStates {
		normal, active, disabled, drag
	};

	private ArrayList<SVGImagePhase> phases = new ArrayList<SVGImagePhase>();
	private int selectedPhase = 0;

	public SVGImage(Control parent, String... imageNames) {
		super(parent);

		for (String s : imageNames)
			phases.add(createPhase(s));
	}

	protected SVGImagePhase createPhase(String s) {
		return new SVGImagePhase(this, s);
	}

	public void selectPhase(int idx) {
		if (idx < phases.size())
			if (idx != selectedPhase) {
				selectedPhase = idx;
				requestLayout();
			}
	}

	public int getSelectedPhase() {
		return selectedPhase;
	}

	@Override
	public Control mouseDown(Position eventPoint) {
		return this;
	}

	public void onMouseCaptured() {
		if (hasMouseOver() && phases.size() > 1)
			selectPhase(1);
	}

	public void onMouseLost() {
		if (hasMouseOver() && phases.size() > 0)
			selectPhase(0);
	}

	protected boolean hasMouseOver() {
		return true;
	}

	private static native String base64Encode(String v) /*-{
		return btoa(v);
	}-*/;

	private static native String base64Decode(String v) /*-{
		return atob(v);
	}-*/;

	@Override
	public void calcPixRect(Position parentsReference, double currentZoom) {
		super.calcPixRect(parentsReference, currentZoom);

		SVGImagePhase phase = getSelectedImage();

		if (phase != null)
			phase.calcPixRect(getPixRect().getLeftTop(), currentZoom);
	}

	@Override
	public void doLayout(double x, double y, double w, double h) {
		super.doLayout(x, y, w, h);

		SVGImagePhase phase = getSelectedImage();

		if (phase != null) {
			double xOff = (w - phase.getImgWidth()) / 2;
			double yOff = (h - phase.getImgHeight()) / 2;
			phase.doLayout(xOff, yOff, w, h);
		}
	}

	static public double calcSVGDimensionToPixels(double svgDim) {
		return SVGImagePhase.calcSVGDimensionToPixels(svgDim);
	}

	public SVGImagePhase getSelectedImage() {
		return getPhase(getSelectedPhase());
	}

	protected SVGImagePhase getPhase(int phase) {
		if (phase >= 0 && phase < phases.size())
			return phases.get(phase);

		return null;
	}

	@Override
	public void draw(Context2d ctx, int invalidationMask) {
		SVGImagePhase phase = getSelectedImage();

		if (phase != null) {
			phase.draw(ctx, invalidationMask);
		}
	}

	protected double getMinWidth() {
		return 0;
	}

	protected double getMinHeight() {
		return 0;
	}

	protected Rect getBitmapRect() {
		Rect r = getPixRect().copy();
		r.applyPadding(getLeftPadding(), getTopPadding(), getRightPadding(), getBottomPadding());
		return r;
	}

	protected double getPadding() {
		return 0;
	}

	protected double getXPadding() {
		return getPadding();
	}

	protected double getYPadding() {
		return getPadding();
	}

	protected double getLeftPadding() {
		return getXPadding();
	}

	protected double getRightPadding() {
		return getXPadding();
	}

	protected double getTopPadding() {
		return getYPadding();
	}

	protected double getBottomPadding() {
		return getYPadding();
	}

}
