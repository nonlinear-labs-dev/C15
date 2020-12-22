package com.nonlinearlabs.client.world.maps;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Dimension;
import com.nonlinearlabs.client.world.NonLinearWorld;
import com.nonlinearlabs.client.world.Position;

public abstract class MapsControl extends Control {

	private final NonRect nonPosition = new NonRect();

	public MapsControl(MapsControl parent) {
		super(parent);
	}

	public abstract void doFirstLayoutPass(double levelOfDetail);

	public void doSecondLayoutPass(double parentsWidthFromFirstPass, double parentsHeightFromFirstPass) {
	}

	@Override
	public MapsLayout getParent() {
		return (MapsLayout) super.getParent();
	}

	public NonLinearWorld getWorld() {
		return getParent().getWorld();
	}

	@Override
	public void draw(Context2d ctx, Context2d overlay, Context2d menus, int invalidationMask) {
	}

	@Override
	public void calcPixRect(Position parentsReference, double zoom) {
		final double xu = nonPosition.getPosition().getX() + getLeftMargin();
		final double yu = nonPosition.getPosition().getY() + getTopMargin();
		final double wu = nonPosition.getDimension().getWidth() - getLeftMargin() - getRightMargin();
		final double hu = nonPosition.getDimension().getHeight() - getTopMargin() - getBottomMargin();

		final double x = xu * zoom + parentsReference.getX();
		final double y = yu * zoom + parentsReference.getY();
		final double w = wu * zoom;
		final double h = hu * zoom;

		getPixRect().set(x, y, Math.max(w, 0), Math.max(h, 0));
	}

	public double getXMargin() {
		return 0;
	}

	public double getYMargin() {
		return 0;
	}

	public double getLeftMargin() {
		return getXMargin();
	}

	public double getRightMargin() {
		return getXMargin();
	}

	public double getTopMargin() {
		return getYMargin();
	}

	public double getBottomMargin() {
		return getYMargin();
	}

	public double getWidthMargin() {
		return getLeftMargin() + getRightMargin();
	}

	public double getHeightMargin() {
		return getTopMargin() + getBottomMargin();
	}

	public boolean setNonSize(double width, double height) {
		if (nonPosition.getDimension().set(width, height)) {
			invalidate(INVALIDATION_FLAG_UI_CHANGED);
			return true;
		}
		return false;
	}

	public boolean setNonSize(NonDimension dim) {
		return setNonSize(dim.getWidth(), dim.getHeight());
	}

	public NonRect getNonPosition() {
		return nonPosition;
	}

	public NonRect getNonPositionRelativeToParent(MapsLayout parent) {
		NonRect r = nonPosition.copy();
		MapsControl p = getParent();

		while (p != parent) {
			r.moveBy(p.getNonPosition().getPosition());
			p = p.getParent();
		}

		return r;
	}

	public double getCurrentZoom() {
		return getNonMaps().getNonLinearWorld().getCurrentZoom();
	}

	public double getConturPixels() {
		return getParent().getConturPixels();
	}

	public double toXPixels(double units) {
		return units * getPixRect().getWidth() / getNonPosition().getWidth();
	}

	public double toYPixels(double units) {
		return units * getPixRect().getHeight() / getNonPosition().getHeight();
	}

	public NonDimension toNonDimension(Dimension dim) {
		return toNonDimension(dim, getCurrentZoom());
	}

	public NonPosition toNonPosition(Position pos) {
		double zoom = getCurrentZoom();
		double x = pos.getX() / zoom + getNonMaps().getNonLinearWorld().getViewport().getNonPosition().getLeft();
		double y = pos.getY() / zoom + getNonMaps().getNonLinearWorld().getViewport().getNonPosition().getTop();
		return new NonPosition(x, y);
	}

	private NonDimension toNonDimension(Dimension dim, double zoom) {
		double width = dim.getWidth() / zoom;
		double height = dim.getHeight() / zoom;
		return new NonDimension(width, height);
	}

	public double getLevelOfDetailForFullVisibility() {
		return getParent().getLevelOfDetailForFullVisibility();
	}

	public double getCurrentLevelOfDetail() {
		return getParent().getCurrentLevelOfDetail();
	}

	public double getMaxLevelOfDetail() {
		return getLevelOfDetailForFullVisibility();
	}

	public boolean moveTo(double x, double y) {
		return getNonPosition().moveTo(x, y);
	}

	public boolean moveTo(NonPosition pos) {
		return getNonPosition().moveTo(pos.getX(), pos.getY());
	}

	public void scrollToMakeFullyVisible() {
		NonMaps.theMaps.getNonLinearWorld().scrollToShow(this);
	}

	public boolean zoomTo(MapsControl ctrl) {
		return getParent().zoomTo(ctrl);
	}
}
