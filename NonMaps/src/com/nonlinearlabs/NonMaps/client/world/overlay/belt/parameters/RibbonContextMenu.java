package com.nonlinearlabs.NonMaps.client.world.overlay.belt.parameters;

import com.nonlinearlabs.NonMaps.client.world.Control;
import com.nonlinearlabs.NonMaps.client.world.Position;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.PhysicalControlParameter.ReturnMode;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.PlayControls.HardwareSources.Ribbon;
import com.nonlinearlabs.NonMaps.client.world.overlay.ContextMenuItem;
import com.nonlinearlabs.NonMaps.client.world.overlay.OverlayLayout;

public class RibbonContextMenu extends ParameterContextMenu {

	public RibbonContextMenu(OverlayLayout parent, final Ribbon param) {
		super(parent, param);

		ReturnMode mode = param.getReturnMode();
		
		String noneString = mode == ReturnMode.None ?  "\uE0A4 Non-Return" : "\uE0A3 Non-Return";
		String centerString = mode == ReturnMode.Center ?  "\uE0A4 Return to center" : "\uE0A3 Return to center";
		
		addChild(new ContextMenuItem(this, noneString) {
			@Override
			public Control click(Position eventPoint) {
				getNonMaps().getServerProxy().setRibbonReturnMode(param.getParameterID(), "stay");
				return super.click(eventPoint);
			}
		});

		addChild(new ContextMenuItem(this, centerString) {
			@Override
			public Control click(Position eventPoint) {
				getNonMaps().getServerProxy().setRibbonReturnMode(param.getParameterID(), "return");
				return super.click(eventPoint);
			}
		});
	}
}
