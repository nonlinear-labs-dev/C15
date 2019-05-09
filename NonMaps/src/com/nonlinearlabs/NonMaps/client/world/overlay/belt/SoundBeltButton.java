package com.nonlinearlabs.NonMaps.client.world.overlay.belt;

import com.nonlinearlabs.NonMaps.client.world.Control;
import com.nonlinearlabs.NonMaps.client.world.Position;
import com.nonlinearlabs.NonMaps.client.world.overlay.OverlayLayout;

public class SoundBeltButton extends BeltButton {

	public SoundBeltButton(OverlayLayout parent, Belt belt) {
		super(parent, belt, "Sound_Tab_Enabled_A.svg", "Sound_Tab_Disabled_A.svg");
	}

	@Override
	public Control click(Position eventPoint) {
		if (!isActive()) {
			belt.setSoundView(true);
		} else {
			belt.toggle();
		}
		return this;
	}

	@Override
	public boolean isActive() {
		return belt.isSoundView();
	}
}
