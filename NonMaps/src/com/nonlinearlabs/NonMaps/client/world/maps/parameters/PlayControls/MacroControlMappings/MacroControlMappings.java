package com.nonlinearlabs.NonMaps.client.world.maps.parameters.PlayControls.MacroControlMappings;

import com.nonlinearlabs.NonMaps.client.NonMaps;
import com.nonlinearlabs.NonMaps.client.world.Control;
import com.nonlinearlabs.NonMaps.client.world.Position;
import com.nonlinearlabs.NonMaps.client.world.maps.MapsLayout;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.LabelModuleHeader;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.ParameterGroupVertical;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.PhysicalControlParameter;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.PlayControls.HardwareSources.Pedal;
import com.nonlinearlabs.NonMaps.client.world.overlay.ContextMenu;
import com.nonlinearlabs.NonMaps.client.world.overlay.ContextMenuItem;
import com.nonlinearlabs.NonMaps.client.world.overlay.Overlay;
import com.nonlinearlabs.NonMaps.client.world.overlay.OverlayLayout;
import com.nonlinearlabs.NonMaps.client.world.overlay.setup.ContextMenusSetting;

public class MacroControlMappings extends ParameterGroupVertical {

	private MacroControlMappingControls controls;

	public class MacroControlMappingsGroupContextMenu extends ContextMenu {
		public MacroControlMappingsGroupContextMenu(OverlayLayout parent) {
			super(parent);

			addChild(new ContextMenuItem(this, (isHWLocked() ? "Unlock" : "Lock") + " Hardware Sources") {
				@Override
				public Control click(Position eventPoint) {
					NonMaps.get().getServerProxy().toggleGroupLock("CS");
					return super.click(eventPoint);
				}
			});

			addChild(new ContextMenuItem(this, (isAmountsLocked() ? "Unlock" : "Lock") + " Amounts") {
				@Override
				public Control click(Position eventPoint) {
					NonMaps.get().getServerProxy().toggleGroupLock("MCM");
					return super.click(eventPoint);
				}
			});

			addChild(new ContextMenuItem(this, "Lock all groups") {
				@Override
				public Control click(Position eventPoint) {
					NonMaps.get().getServerProxy().lockAllGroups();
					return super.click(eventPoint);
				}
			});

			if (NonMaps.get().getNonLinearWorld().getParameterEditor().isAnyParameterLocked()) {
				addChild(new ContextMenuItem(this, "Unlock all groups") {
					@Override
					public Control click(Position eventPoint) {
						NonMaps.get().getServerProxy().unlockAllGroups();
						return super.click(eventPoint);
					}
				});
			}
		}
	}

	public MacroControlMappings(MapsLayout parent) {
		super(parent);
		addChild(new LabelModuleHeader(this, "Hardware Sources and Amounts") {

			@Override
			public Control onContextMenu(Position pos) {
				ContextMenusSetting contextMenuSettings = NonMaps.theMaps.getNonLinearWorld().getViewport().getOverlay().getSetup()
						.getContextMenuSettings();

				if (contextMenuSettings.isEnabled()) {
					Overlay o = NonMaps.theMaps.getNonLinearWorld().getViewport().getOverlay();
					ContextMenu c = new MacroControlMappingsGroupContextMenu(o);
					return o.setContextMenu(pos, c);
				}
				return super.onContextMenu(pos);
			}

			@Override
			protected boolean isLocked() {
				return super.isLocked();
			}
		});

		addChild(controls = new MacroControlMappingControls(this));
	}

	@Override
	public String getID() {
		return "MCM";
	}

	public void onReturningModeChanged(PhysicalControlParameter src) {
		controls.onReturningModeChanged(src);
	}

	public Pedal getPedal(int i) {
		return controls.getPedal(i);
	}

	private boolean isAmountsLocked() {
		return controls.isAmountsLocked();
	}

	private boolean isHWLocked() {
		return controls.isHWLocked();
	}
}
