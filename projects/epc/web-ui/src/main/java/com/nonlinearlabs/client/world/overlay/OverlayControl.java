package com.nonlinearlabs.client.world.overlay;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.Rect;

public abstract class OverlayControl extends Control {

	private Rect relativePosition = new Rect();

	public OverlayControl(Control parent) {
		super(parent);
	}

	public Rect getRelativePosition() {
		return relativePosition;
	}

	public void setRelativePosition(Rect relativePosition) {
		this.relativePosition = relativePosition;
		this.relativePosition.getDimension().makeNonNegative();
	}

	public void doLayout(double x, double y, double w, double h) {
		getRelativePosition().getDimension().set(w, h);
		getRelativePosition().getPosition().setX(x);
		getRelativePosition().getPosition().setY(y);
	}

	public Rect getPositionRelativeToParent(OverlayControl parent) {
		Rect r = relativePosition.copy();
		OverlayControl p = (OverlayControl) getParent();

		while (p != parent) {
			r.moveBy(p.getRelativePosition().getPosition());
			p = (OverlayControl) p.getParent();
		}

		return r;
	}

	@Override
	public void calcPixRect(Position parentsReference, double currentZoom) {
		setPixRect(getCalculatedPixRect(parentsReference, currentZoom));
	}

	public Rect getCalculatedPixRect(Position parentsReference, double currentZoom) {
		Rect newPos = getRelativePosition().copy();
		newPos.getPosition().moveBy(parentsReference);
		return newPos;
	}

	@Override
	public abstract void draw(Context2d ctx, Context2d overlay, int invalidationMask);

	void center() {
		Rect myPos = getRelativePosition();
		Rect parentPos = ((OverlayControl) getParent()).getRelativePosition();

		myPos.setLeft((parentPos.getWidth() - myPos.getWidth()) / 2);
		myPos.setTop((parentPos.getHeight() - myPos.getHeight()) / 2);
	}

}
