package com.nonlinearlabs.NonMaps.client.world.overlay.belt.parameters;

import com.nonlinearlabs.NonMaps.client.world.Control;
import com.nonlinearlabs.NonMaps.client.world.Position;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.PhysicalControlParameter.ReturnMode;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.PlayControls.HardwareSources.Pedal;
import com.nonlinearlabs.NonMaps.client.world.overlay.ContextMenuItem;
import com.nonlinearlabs.NonMaps.client.world.overlay.OverlayLayout;

public class PedalContextMenu extends ParameterContextMenu {
	
	public PedalContextMenu(OverlayLayout parent, final Pedal param) {
		super(parent, param);

		ReturnMode mode = param.getReturnMode();
		
		String noneString = mode == ReturnMode.None ?  "\uE0A4 Non-Return" : "\uE0A3 Non-Return";
		String zeroString = mode == ReturnMode.Zero ?  "\uE0A4 Return to zero" : "\uE0A3 Return to zero";
		String centerString = mode == ReturnMode.Center ?  "\uE0A4 Return to center" : "\uE0A3 Return to center";

		addChild(new ContextMenuItem(this, noneString) {
			@Override
			public Control click(Position eventPoint) {
				getNonMaps().getServerProxy().setPedalMode(param.getParameterID(), "stay");
				return super.click(eventPoint);
			}
		});
		
		addChild(new ContextMenuItem(this, zeroString) {
			@Override
			public Control click(Position eventPoint) {
				getNonMaps().getServerProxy().setPedalMode(param.getParameterID(), "return-to-zero");
				return super.click(eventPoint);
			}
		});

		addChild(new ContextMenuItem(this, centerString) {
			@Override
			public Control click(Position eventPoint) {
				getNonMaps().getServerProxy().setPedalMode(param.getParameterID(), "return-to-center");
				return super.click(eventPoint);
			}
		});
	}
}
