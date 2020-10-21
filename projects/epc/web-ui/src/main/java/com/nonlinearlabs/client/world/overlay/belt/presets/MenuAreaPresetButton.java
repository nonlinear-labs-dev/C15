package com.nonlinearlabs.client.world.overlay.belt.presets;

import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.presenters.BankPresenterProviders;
import com.nonlinearlabs.client.presenters.PresetManagerPresenterProvider;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.maps.presets.PresetManager;
import com.nonlinearlabs.client.world.overlay.Overlay;
import com.nonlinearlabs.client.world.overlay.SVGImage;

public class MenuAreaPresetButton extends SVGImage {

	public MenuAreaPresetButton(MenuArea parent) {
		super(parent, "Menu_Preset_Enabled_S.svg", "Menu_Preset_Active_S.svg", "Menu_Preset_Disabled_S.svg",
				"Menu_A_Enabled.svg", "Menu_A_Active.svg", "Menu_A_Disabled.svg");
	}

	@Override
	public Control mouseDown(Position pos) {
		var p = getPreset();
		Overlay o = getOverlay();

		if (o.getContextMenu() instanceof PresetContextMenu) {
			o.removeExistingContextMenus();
			return this;
		}

		if (p != null) {
			Position po = getPixRect().getLeftTop();
			PresetContextMenu pm = new PresetContextMenu(o, p);
			po.moveBy(3, -pm.getDesiredHeight() + 4);
			return o.setContextMenu(po, pm);
		}
		return super.click(pos);
	}

	private Overlay getOverlay() {
		return NonMaps.get().getNonLinearWorld().getViewport().getOverlay();
	}

	@Override
	public Control onContextMenu(Position pos) {
		return mouseDown(pos);
	}

	String getPreset() {
		return PresetManagerPresenterProvider.get().getPresenter().selectedPreset;
	}

	private PresetManager getPresetManager() {
		return NonMaps.get().getNonLinearWorld().getPresetManager();
	}

	boolean hasPreset() {
		var bankUuid = PresetManagerPresenterProvider.get().getPresenter().selectedBank;
		var bank = BankPresenterProviders.get().getPresenter(bankUuid);
		return !bank.presets.isEmpty();
	}

	@Override
	public int getSelectedPhase() {
		int ret = 0;

		if (hasPreset())
			ret = drawStates.normal.ordinal();
		else
			ret = drawStates.disabled.ordinal();

		if (((MenuArea) getParent()).isSmall() == false)
			ret += 3;
		return ret;
	}
}
