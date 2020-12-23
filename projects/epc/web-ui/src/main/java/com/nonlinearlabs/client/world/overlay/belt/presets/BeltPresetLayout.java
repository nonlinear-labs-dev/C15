package com.nonlinearlabs.client.world.overlay.belt.presets;

import com.google.gwt.dom.client.Style.Display;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.overlay.OverlayLayout;
import com.nonlinearlabs.client.world.overlay.belt.Belt;
import com.nonlinearlabs.client.world.overlay.belt.presets.html.BeltPresetsUI;

public class BeltPresetLayout extends OverlayLayout {
	private BeltPresetsUI theHtmlUI = new BeltPresetsUI();

	public BeltPresetLayout(Belt parent) {
		super(parent);

	}

	@Override
	public void calcPixRect(Position parentsReference, double currentZoom) {
		super.calcPixRect(parentsReference, currentZoom);
		theHtmlUI.syncPosition(getPixRect());
	}

	@Override
	public void movePixRect(double x, double y) {
		super.movePixRect(x, y);
		theHtmlUI.syncPosition(getPixRect());
	}

	@Override
	public void fadeIn() {
		super.fadeIn();
		theHtmlUI.getElement().getStyle().setDisplay(Display.FLEX);
	}

	@Override
	public void fadeOut() {
		super.fadeOut();
		theHtmlUI.getElement().getStyle().setDisplay(Display.NONE);
	}

}