package com.nonlinearlabs.client.world.overlay.belt;

import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.IPreset;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.overlay.DragProxy;
import com.nonlinearlabs.client.world.overlay.OverlayLayout;
import com.nonlinearlabs.client.world.overlay.belt.Belt.BeltTab;

public class SoundBeltButton extends BeltButton {

	public SoundBeltButton(OverlayLayout parent, Belt belt) {
		super(parent, belt, "Sound_Tab_Enabled_A.svg", "Sound_Tab_Disabled_A.svg");
	}

	@Override
	public Control click(Position eventPoint) {
		if (!isSoundActive()) {
			belt.setSoundView(true);
		} else {
			belt.toggle();
		}
		return this;
	}

	public boolean isSoundActive() {
		return belt.isSoundView();
	}

	@Override
	public boolean isActive() {
		return isSoundActive() || belt.isFadeView();
	}


	@Override
	public Control drag(Position p, DragProxy dragProxy) {
		if(getPixRect().contains(p) && dragProxy.getOrigin() instanceof IPreset)
		{
			if(belt.isHidden())
				belt.toggle();
				
			belt.openTab(BeltTab.Sound);
			return this;
		}
		return super.drag(p, dragProxy);
	}
}
