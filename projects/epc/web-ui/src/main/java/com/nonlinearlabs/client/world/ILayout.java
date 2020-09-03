package com.nonlinearlabs.client.world;

import java.util.ArrayList;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.world.Control.ControlFinder;

public interface ILayout<TChild extends Control> {
	public ArrayList<TChild> getChildren();

	public <T extends TChild> T addChild(T child);

	public <T extends TChild> T insertChild(int idx, T child);

	public <T extends TChild> T prependChild(T child);

	public void drawChildren(Context2d ctx, Context2d overlay, int invalidationMask);

	public void calcPixRect(Position parentsReference, double currentZoom);

	public void movePixRect(double x, double y);

	public Control recurseChildren(ControlFinder handler);

	public Control recurseChildren(Position pos, ControlFinder handler);
}
