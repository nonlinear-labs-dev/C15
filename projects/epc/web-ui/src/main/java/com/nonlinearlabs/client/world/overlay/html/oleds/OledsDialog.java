package com.nonlinearlabs.client.world.overlay.html.oleds;

import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.world.overlay.GWTDialog;

public class OledsDialog extends GWTDialog {

	static int lastPopupLeft = -1;
	static int lastPopupTop = -1;
	static OledsDialog theOleds;

	Oleds oledsView = null;

	private OledsDialog() {
		super();
		add(oledsView = new Oleds());
		getElement().addClassName("oleds-dialog");
		setAnimationEnabled(true);
		setGlassEnabled(false);
		setModal(false);
	}

	public static void showDialog() {
		if (theOleds == null) {
			theOleds = new OledsDialog();
			theOleds.setModal(false);
			theOleds.addHeader("C15 Oleds");
			theOleds.initalShow();
			theOleds.pushDialogToFront();
		}
	}

	public static void hideDialog() {
		if (theOleds != null)
			theOleds.commit();
	}

	public static void toggle() {
		if (theOleds != null)
			hideDialog();
		else
			showDialog();
	}

	public static boolean isShown() {
		return theOleds != null;
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
		theOleds = null;
		NonMaps.theMaps.captureFocus();
		NonMaps.theMaps.getNonLinearWorld().requestLayout();
	}
}
