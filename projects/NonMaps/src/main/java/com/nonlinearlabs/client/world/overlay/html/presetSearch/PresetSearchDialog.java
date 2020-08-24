package com.nonlinearlabs.client.world.overlay.html.presetSearch;

import com.google.gwt.event.dom.client.KeyCodes;
import com.google.gwt.user.client.Event.NativePreviewEvent;
import com.google.gwt.user.client.Event;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.world.overlay.GWTDialog;

public class PresetSearchDialog extends GWTDialog {

	static int lastPopupLeft = -1;
	static int lastPopupTop = -1;
	static PresetSearchDialog theSearch;

	PresetSearch presetSearch = null;

	private PresetSearchDialog() {
		super();
		com.nonlinearlabs.client.useCases.PresetSearch.get().startSearch();
		add(presetSearch = new PresetSearch());
		getElement().addClassName("search-dialog");
		setAnimationEnabled(true);
		setGlassEnabled(false);
		setModal(false);
	}

	public static void showDialog() {
		if (theSearch == null) {
			theSearch = new PresetSearchDialog();
			theSearch.setModal(false);
			theSearch.addHeader("Search Presets for ...");
			theSearch.initalShow();
			theSearch.pushDialogToFront();
			NonMaps.get().getNonLinearWorld().getPresetManager().closeMultiSelection();
		}
	}

	@Override
	protected void onPreviewNativeEvent(NativePreviewEvent event) {
		super.onPreviewNativeEvent(event);
		switch (event.getTypeInt()) {
		case Event.ONKEYDOWN:
			if (event.getNativeEvent().getKeyCode() == KeyCodes.KEY_ESCAPE) {
				hideDialog();
			}
			break;
		}
	}

	public static void hideDialog() {
		if (theSearch != null) {
			theSearch.commit();
		}
	}

	public static void toggle() {
		if (theSearch != null)
			hideDialog();
		else
			showDialog();
	}

	public static boolean isShown() {
		return theSearch != null;
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
		com.nonlinearlabs.client.useCases.PresetSearch.get().finishSearch();
		theSearch = null;
		NonMaps.theMaps.captureFocus();
		NonMaps.theMaps.getNonLinearWorld().requestLayout();
	}
}
