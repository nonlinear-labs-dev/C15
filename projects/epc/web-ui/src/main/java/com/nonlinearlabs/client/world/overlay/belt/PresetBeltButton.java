package com.nonlinearlabs.client.world.overlay.belt;

import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.overlay.DragProxy;
import com.nonlinearlabs.client.world.overlay.OverlayLayout;
import com.nonlinearlabs.client.world.overlay.belt.Belt.BeltTab;

public class PresetBeltButton extends BeltButton {

	public PresetBeltButton(OverlayLayout parent, Belt belt) {
		super(parent, belt, "Preset_Tab_Enabled.svg", "Preset_Tab_Disabled.svg");
	}

	@Override
	public Control click(Position eventPoint) {
		if (!isActive()) {
			belt.setPresetView(true);
		} else {
			belt.toggle();
		}
		return this;
	}

	@Override
	public boolean isActive() {
		return belt.isPresetView();
	}

	@Override
	public Control drag(Position p, DragProxy dragProxy) {
		// todo
		// if(getPixRect().contains(p) && dragProxy.getOrigin() instanceof IPreset)
		{
			if (belt.isHidden())
				belt.toggle();

			belt.openTab(BeltTab.Preset);
			return this;
		}
		// return super.drag(p, dragProxy);
	}
}
