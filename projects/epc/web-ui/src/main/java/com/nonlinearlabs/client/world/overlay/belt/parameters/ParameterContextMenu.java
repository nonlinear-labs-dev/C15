package com.nonlinearlabs.client.world.overlay.belt.parameters;

import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.dataModel.setup.SetupModel;
import com.nonlinearlabs.client.useCases.EditBufferUseCases;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.maps.parameters.Parameter;
import com.nonlinearlabs.client.world.overlay.ContextMenu;
import com.nonlinearlabs.client.world.overlay.ContextMenuItem;
import com.nonlinearlabs.client.world.overlay.OverlayLayout;
import com.nonlinearlabs.client.world.overlay.ParameterInfoDialog;
import com.nonlinearlabs.client.world.overlay.belt.Belt;

public class ParameterContextMenu extends ContextMenu {

	public ParameterContextMenu(OverlayLayout parent, final Parameter param) {
		super(parent);

		addChild(new ContextMenuItem(this, "Parameter Info ...") {
			@Override
			public Control click(Position eventPoint) {
				ParameterInfoDialog.toggle();
				invalidate(INVALIDATION_FLAG_UI_CHANGED);
				return super.click(eventPoint);
			}
		});

		if(param.getParameterNumber() == 356) {
			boolean state = SetupModel.get().systemSettings.syncSplit.getBool();
			addChild(new ContextMenuItem(this, state ? "Unlink Splitpoints" : "Link Splitpoints") {
				@Override
				public Control click(Position eventPoint) {
					EditBufferUseCases.get().toggleSyncSplit();
					return super.click(eventPoint);
				}
			});
		}

		Belt belt = NonMaps.get().getNonLinearWorld().getViewport().getOverlay().getBelt();
		if (belt.isHidden() || !belt.isParameterView()) {

			addChild(new ContextMenuItem(this, "Open Tab") {
				@Override
				public Control click(Position eventPoint) {

					belt.openTab(Belt.BeltTab.Parameter);
					if (belt.isHidden())
						belt.toggle();

					return super.click(eventPoint);
				}
			});
		}
	}
}
