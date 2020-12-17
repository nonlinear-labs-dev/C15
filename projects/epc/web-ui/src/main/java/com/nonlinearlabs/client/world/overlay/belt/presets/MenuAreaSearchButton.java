package com.nonlinearlabs.client.world.overlay.belt.presets;

import com.nonlinearlabs.client.presenters.PresetManagerPresenterProvider;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.overlay.SVGImage;
import com.nonlinearlabs.client.world.overlay.html.presetSearch.PresetSearchDialog;

public class MenuAreaSearchButton extends SVGImage {

	public MenuAreaSearchButton(MenuArea parent) {
		super(parent, "Menu_Search_Enabled_S.svg", "Menu_Search_Active_S.svg", "Menu_Search_Disabled_S.svg");
	}

	@Override
	public Control click(Position eventPoint) {
		toggleSearch();
		return this;
	}

	private void toggleSearch() {
		PresetSearchDialog.toggle();
	}

	@Override
	public int getSelectedPhase() {
		if (PresetManagerPresenterProvider.get().getValue().hasPresets)
			return drawStates.disabled.ordinal();

		return PresetSearchDialog.isShown() ? drawStates.active.ordinal() : drawStates.normal.ordinal();
	}
}
