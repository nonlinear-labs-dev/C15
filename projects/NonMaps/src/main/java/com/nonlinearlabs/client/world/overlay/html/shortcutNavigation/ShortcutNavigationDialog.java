package com.nonlinearlabs.client.world.overlay.html.shortcutNavigation;

import com.google.gwt.core.client.Scheduler;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.world.overlay.GWTDialog;

public class ShortcutNavigationDialog extends GWTDialog {

	static int lastPopupLeft = -1;
	static int lastPopupTop = -1;
	static ShortcutNavigationDialog theSetup;

	private ShortcutNavigationDialog() {
		super();
		ShortcutNavigation n = new ShortcutNavigation(this);
		add(n);
		getElement().addClassName("shortcut-navigation-dialog");
		setAnimationEnabled(true);
		setGlassEnabled(false);
		setModal(false);
		Scheduler.get().scheduleDeferred(() -> n.setFocus());
	}

	public static void showDialog() {
		if (theSetup == null) {
			theSetup = new ShortcutNavigationDialog();
			theSetup.setModal(false);
			theSetup.addHeader("Shortcuts");
			theSetup.initalShow();
		}
	}

	public static void hideDialog() {
		if (theSetup != null) {
			theSetup.commit();
		}
	}

	public static void toggle() {
		if (theSetup != null)
			hideDialog();
		else
			showDialog();
	}

	public static boolean isShown() {
		return theSetup != null;
	}

	@Override
	protected void setLastPopupPos(int popupLeft, int popupTop) {
		lastPopupLeft = popupLeft;
		lastPopupTop = popupTop;
	}

	@Override
	protected int getLastPopupPosTop() {
		return lastPopupTop;
	}

	@Override
	protected int getLastPopupPosLeft() {
		return lastPopupLeft;
	}

	@Override
	protected void commit() {
		hide();
		theSetup = null;
		NonMaps.theMaps.captureFocus();
		NonMaps.theMaps.getNonLinearWorld().requestLayout();
	}
}
