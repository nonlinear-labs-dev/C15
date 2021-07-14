package com.nonlinearlabs.client.world.overlay.html.randomizeSound;

import com.google.gwt.event.dom.client.KeyCodes;
import com.google.gwt.user.client.Event;
import com.google.gwt.user.client.Event.NativePreviewEvent;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.SoundType;
import com.nonlinearlabs.client.world.overlay.GWTDialog;

public class RandomizeSoundDialog extends GWTDialog {

	static int lastPopupLeft = -1;
	static int lastPopupTop = -1;
	static RandomizeSoundDialog theRandomize;

	RandomizeSound presetSearch = null;

	private RandomizeSoundDialog() {
		super();
		add(presetSearch = new RandomizeSound());
		getElement().addClassName("randomize-sound-dialog");
		setAnimationEnabled(true);
		setGlassEnabled(false);
		setModal(false);

		addHeader("Randomize");
		initalShow();
		pushDialogToFront();

		EditBufferModel.get().soundType.onChange(v -> {
			updateHeader();
			return true;
		});

		EditBufferModel.get().voiceGroup.onChange(v -> {
			updateHeader();
			return true;
		});
	}

	public static void showDialog() {
		if (theRandomize == null) {
			theRandomize = new RandomizeSoundDialog();
		}
	}

	private void updateHeader() {
		if (EditBufferModel.get().soundType.getValue() == SoundType.Single)
			setHeaderText("Randomize Sound");
		else
			setHeaderText("Randomize Part " + EditBufferModel.get().voiceGroup.getValue().toString());
	}

	public static void hideDialog() {
		if (theRandomize != null) {
			theRandomize.commit();
		}
	}

	public static void toggle() {
		if (theRandomize != null)
			hideDialog();
		else
			showDialog();
	}

	public static boolean isShown() {
		return theRandomize != null;
	}

	@Override
	protected void setLastPopupPos(int popupLeft, int popupTop) {
		lastPopupLeft = popupLeft;
		lastPopupTop = popupTop;
	}

	@Override
	protected void onPreviewNativeEvent(NativePreviewEvent event) {
		super.onPreviewNativeEvent(event);
		switch (event.getTypeInt()) {
		case Event.ONKEYDOWN:
			if (event.getNativeEvent().getKeyCode() == KeyCodes.KEY_ESCAPE) {
				hide();
			}
			break;
		}
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
		theRandomize = null;
		NonMaps.theMaps.captureFocus();
		NonMaps.theMaps.getNonLinearWorld().requestLayout();
	}
}
