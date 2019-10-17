package com.nonlinearlabs.NonMaps.client.world.overlay.belt.sound;

import com.nonlinearlabs.NonMaps.client.world.overlay.OverlayLayout;

public class SingleSoundLayout extends SoundLayout {

	protected SingleSoundLayout(OverlayLayout parent) {
		super(parent);
		setSettings(new SingleSoundSettings(this));
	}
}
