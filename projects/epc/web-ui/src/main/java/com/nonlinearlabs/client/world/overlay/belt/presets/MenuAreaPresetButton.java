package com.nonlinearlabs.client.world.overlay.belt.presets;

import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.maps.presets.PresetManager;
import com.nonlinearlabs.client.world.maps.presets.bank.Bank;
import com.nonlinearlabs.client.world.maps.presets.bank.preset.Preset;
import com.nonlinearlabs.client.world.overlay.Overlay;
import com.nonlinearlabs.client.world.overlay.SVGImage;

public class MenuAreaPresetButton extends SVGImage {

	public MenuAreaPresetButton(MenuArea parent) {
		super(parent, "Menu_Preset_Enabled_S.svg", "Menu_Preset_Active_S.svg", "Menu_Preset_Disabled_S.svg",
				"Menu_A_Enabled.svg", "Menu_A_Active.svg", "Menu_A_Disabled.svg");
	}

	@Override
	public Control mouseDown(Position pos) {
		Preset p = getPreset();
		Overlay o = getOverlay();

		if (o.getContextMenu() instanceof PresetContextMenu) {
			o.removeExistingContextMenus();
			return this;
		}

		if (p != null) {
			Position po = getPixRect().getLeftTop();
			PresetContextMenu pm = new PresetContextMenu(o, p.getUUID());
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

	Preset getPreset() {
		PresetManager pm = getPresetManager();
		String bankUUID = pm.getSelectedBank();

		if (bankUUID != null) {
			Bank bank = pm.findBank(bankUUID);
			if (bank != null) {
				String presetUUID = bank.getPresetList().getSelectedPreset();
				return bank.getPresetList().findPreset(presetUUID);
			}
		}
		return null;
	}

	private PresetManager getPresetManager() {
		return NonMaps.get().getNonLinearWorld().getPresetManager();
	}

	boolean hasPreset() {
		PresetManager pm = getPresetManager();
		String bankUUID = pm.getSelectedBank();

		if (bankUUID != null) {
			Bank bank = pm.findBank(bankUUID);
			if (bank != null) {
				String presetUUID = bank.getPresetList().getSelectedPreset();
				return bank.getPresetList().findPreset(presetUUID) != null;
			}
		}
		return false;
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
