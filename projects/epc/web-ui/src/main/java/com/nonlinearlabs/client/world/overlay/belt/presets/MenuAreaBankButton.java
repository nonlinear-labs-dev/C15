package com.nonlinearlabs.client.world.overlay.belt.presets;

import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.presenters.PresetManagerPresenterProvider;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.maps.presets.PresetManager;
import com.nonlinearlabs.client.world.overlay.Overlay;
import com.nonlinearlabs.client.world.overlay.SVGImage;

public class MenuAreaBankButton extends SVGImage {

	public MenuAreaBankButton(MenuArea parent) {
		super(parent, "Menu_Bank_Enabled_S.svg", "Menu_Bank_Active_S.svg", "Menu_Bank_Disabled_S.svg",
				"Menu_A_Enabled.svg", "Menu_A_Active.svg", "Menu_A_Disabled.svg");
	}

	@Override
	public Control mouseDown(Position pos) {
		Overlay o = getOverlay();

		var selectedBank = PresetManagerPresenterProvider.get().getValue().selectedBank;

		if (o.getContextMenu() instanceof CombinedBankContextMenu) {
			o.removeExistingContextMenus();
			return this;
		}

		if (!selectedBank.isEmpty()) {
			CombinedBankContextMenu cm = new CombinedBankContextMenu(o, selectedBank);
			Position p = getPixRect().getLeftTop();
			p.moveBy(3, -cm.getDesiredHeight() + 4);
			return o.setContextMenu(p, cm);
		}
		return o.setContextMenu(getPixRect().getLeftTop(), new CombinedBankContextMenu(o, null));
	}

	private PresetManager getPresetManager() {
		return NonMaps.get().getNonLinearWorld().getPresetManager();
	}

	private Overlay getOverlay() {
		return NonMaps.get().getNonLinearWorld().getViewport().getOverlay();
	}

	@Override
	public Control onContextMenu(Position pos) {
		return mouseDown(pos);
	}

	boolean hasBank() {
		return !PresetManagerPresenterProvider.get().getValue().banks.isEmpty();
	}

	@Override
	public int getSelectedPhase() {
		if (((MenuArea) getParent()).isSmall() == false)
			return drawStates.normal.ordinal() + 3;
		return drawStates.normal.ordinal();
	}
}
