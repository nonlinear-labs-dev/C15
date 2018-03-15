package com.nonlinearlabs.NonMaps.client.world.overlay.belt.parameters;

import com.nonlinearlabs.NonMaps.client.NonMaps;
import com.nonlinearlabs.NonMaps.client.world.Control;
import com.nonlinearlabs.NonMaps.client.world.Position;
import com.nonlinearlabs.NonMaps.client.world.RenameDialog;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.PlayControls.MacroControls.MacroControlParameter;
import com.nonlinearlabs.NonMaps.client.world.overlay.ContextMenuItem;
import com.nonlinearlabs.NonMaps.client.world.overlay.OverlayLayout;
import com.nonlinearlabs.NonMaps.client.world.overlay.ParameterInfoDialog;

public class MacroControlContextMenu extends ParameterContextMenu {

	public MacroControlContextMenu(OverlayLayout parent, final MacroControlParameter param) {
		super(parent, param);

		addChild(new ContextMenuItem(this, "Rename ...") {
			@Override
			public Control click(Position eventPoint) {
				RenameDialog.open(param);
				return super.click(eventPoint);
			}
		});

		String infoText = "Parameter Info ...";

		addChild(new ContextMenuItem(this, infoText) {
			@Override
			public Control click(Position eventPoint) {
				ParameterInfoDialog.toggle();
				invalidate(INVALIDATION_FLAG_UI_CHANGED);
				return super.click(eventPoint);
			}
		});

		addChild(new ContextMenuItem(this, "Mod Reset") {
			@Override
			public Control click(Position eventPoint) {
				NonMaps.theMaps.getServerProxy().resetModulation(param);
				return super.click(eventPoint);
			}
		});
	}
}
