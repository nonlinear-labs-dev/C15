package com.nonlinearlabs.NonMaps.client.world.maps.settings;

import java.util.function.Function;

import com.nonlinearlabs.NonMaps.client.dataModel.setup.Setup;
import com.nonlinearlabs.NonMaps.client.useCases.SystemSettings;
import com.nonlinearlabs.NonMaps.client.world.Control;
import com.nonlinearlabs.NonMaps.client.world.NonLinearWorld;
import com.nonlinearlabs.NonMaps.client.world.Position;
import com.nonlinearlabs.NonMaps.client.world.maps.ContextMenu;
import com.nonlinearlabs.NonMaps.client.world.maps.ContextMenuItem;
import com.nonlinearlabs.NonMaps.client.world.maps.MapsLayout;
import com.nonlinearlabs.NonMaps.client.world.maps.NonPosition;

public class DebugLevel extends Setting {

	public enum Items {
		SILENT, ERROR, WARNING, INFO, DEBUG, GASSY;

		public String toDisplayString() {
			switch (ordinal()) {
			case 0:
				return "Silent";

			case 1:
				return "Error";

			case 2:
				return "Warning";

			case 3:
				return "Info";

			case 4:
				return "Debug";

			case 5:
				return "Gassy";

			default:
				return "";
			}
		}

	}

	private class DebugLevelContextMenu extends ContextMenu {

		private DebugLevelContextMenu(MapsLayout parent) {
			super(parent);

			addChild(new ContextMenuItem(this, Items.SILENT.toDisplayString()) {
				@Override
				public Control click(Position eventPoint) {
					setValue(Items.SILENT);
					return super.click(eventPoint);
				}
			});

			addChild(new ContextMenuItem(this, Items.ERROR.toDisplayString()) {
				@Override
				public Control click(Position eventPoint) {
					setValue(Items.ERROR);
					return super.click(eventPoint);
				}
			});

			addChild(new ContextMenuItem(this, Items.WARNING.toDisplayString()) {
				@Override
				public Control click(Position eventPoint) {
					setValue(Items.WARNING);
					return super.click(eventPoint);
				}
			});

			addChild(new ContextMenuItem(this, Items.INFO.toDisplayString()) {
				@Override
				public Control click(Position eventPoint) {
					setValue(Items.INFO);
					return super.click(eventPoint);
				}
			});

			addChild(new ContextMenuItem(this, Items.DEBUG.toDisplayString()) {
				@Override
				public Control click(Position eventPoint) {
					setValue(Items.DEBUG);
					return super.click(eventPoint);
				}
			});

			addChild(new ContextMenuItem(this, Items.GASSY.toDisplayString()) {
				@Override
				public Control click(Position eventPoint) {
					setValue(Items.GASSY);
					return super.click(eventPoint);
				}
			});
		}
	}

	DebugLevel(DeveloperSettings parent) {
		super(parent, "Debug Level", Items.WARNING.toDisplayString());

		Setup.get().systemSettings.debugLevel.onChange(new Function<Setup.DebugLevel, Boolean>() {

			@Override
			public Boolean apply(Setup.DebugLevel t) {
				setCurrentValue(Items.values()[t.ordinal()].toDisplayString());
				return true;
			}
		});
	}

	public void setValue(Items value) {
		SystemSettings.get().setDebugLevel(Setup.DebugLevel.values()[value.ordinal()]);
	}

	@Override
	public ContextMenu createMenu(NonPosition pos) {
		NonLinearWorld world = getWorld();
		return world.setContextMenu(new DebugLevelContextMenu(world), pos);
	}

	@Override
	public void setDefault() {
		setValue(Items.WARNING);
	}
}
