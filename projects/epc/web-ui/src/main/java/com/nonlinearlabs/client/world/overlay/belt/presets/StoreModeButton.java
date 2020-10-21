package com.nonlinearlabs.client.world.overlay.belt.presets;

import com.google.gwt.xml.client.Node;
import com.nonlinearlabs.client.presenters.PresetManagerPresenterProvider;
import com.nonlinearlabs.client.useCases.PresetManagerUseCases;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.overlay.OverlayLayout;
import com.nonlinearlabs.client.world.overlay.SVGImage;

public class StoreModeButton extends SVGImage {

	public StoreModeButton(OverlayLayout parent) {
		super(parent, "Select_Enabled.svg", "Select_Active.svg", "Select_Disabled.svg");
	}

	@Override
	public int getSelectedPhase() {
		var pm = PresetManagerPresenterProvider.get().getPresenter();
		if (!pm.hasPresets) {
			return drawStates.disabled.ordinal();
		} else if (PresetManagerPresenterProvider.get().getPresenter().inStoreSelectMode) {
			return drawStates.active.ordinal();
		} else {
			return drawStates.normal.ordinal();
		}
	}

	@Override
	public Control mouseDown(Position eventPoint) {
		return super.mouseDown(eventPoint);
	}

	@Override
	public Control mouseUp(Position eventPoint) {
		requestLayout();
		return super.mouseUp(eventPoint);
	}

	@Override
	public Control click(Position eventPoint) {
		toggle();
		return this;
	}

	public void update(Node settingsNode, Node presetManagerNode) {

	}

	public void toggle() {
		PresetManagerUseCases.get().toggleStoreSelectMode();
	}

	public void storeSelectOff() {
		PresetManagerUseCases.get().endStoreSelectMode();
	}
}
