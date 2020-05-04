package com.nonlinearlabs.client;

import com.nonlinearlabs.client.world.NonLinearWorld;
import com.nonlinearlabs.client.world.maps.MapsControl;
import com.nonlinearlabs.client.world.maps.presets.PresetManager;
import com.nonlinearlabs.client.world.maps.presets.bank.Bank;
import com.nonlinearlabs.client.world.maps.presets.bank.preset.Preset;

public class StoreSelectMode implements CustomPresetSelector {
	private PresetManager m_parent;
	private Preset selectedPreset;
	private Bank selectedBank;
	private final Preset originPreset;

	public StoreSelectMode(PresetManager parent) {
		m_parent = parent;

		originPreset = parent.findLoadedPreset();
		selectedPreset = m_parent.getSelectedPreset();
		selectedBank = m_parent.findBank(m_parent.getSelectedBank());
		NonMaps.get().getServerProxy().setSetting("storemode", "on");
	}

	public void setSelectedPreset(Preset p) {
		selectedPreset = p;
		selectedBank = p != null ? p.getParent() : null;
		updateUI();
	}

	public void setSelectedBank(Bank b) {
		selectedBank = b;
		selectedPreset = b.getPresetList().findPreset(b.getPresetList().getSelectedPreset());
		updateUI();
	}

	public Preset getSelectedPreset() {
		return selectedPreset;
	}

	public boolean canNext() {
		if (selectedPreset != null)
			return selectedPreset.getNumber() < selectedPreset.getParent().getPresetList().getPresetCount();

		return false;
	}

	public boolean canPrev() {
		if (selectedPreset != null)
			return selectedPreset.getNumber() > 1;

		return false;
	}

	public void selectNextPreset() {
		if (canNext()) {
			Bank b = selectedPreset.getParent();
			int idx = selectedPreset.getNumber() - 1;
			Preset next = b.getPreset(idx + 1);
			setSelectedPreset(next);
		}
	}

	public void selectPreviousPreset() {
		if (canPrev()) {
			Bank b = selectedPreset.getParent();
			int idx = selectedPreset.getNumber() - 1;
			Preset next = b.getPreset(idx - 1);
			setSelectedPreset(next);
		}
	}

	public boolean canSelectPreviousBank() {
		if (selectedBank != null) {
			return selectedBank.getParent().canSelectBankWithOrdernumberOffset(selectedBank, -1);
		}
		return false;
	}

	public boolean canSelectNextBank() {
		if (selectedBank != null) {
			return selectedBank.getParent().canSelectBankWithOrdernumberOffset(selectedBank, 1);
		}
		return false;
	}

	public void selectNextBank() {
		if (canSelectNextBank()) {
			int orderNumber = selectedBank.getOrderNumber();
			selectBankWithOrderNumber(orderNumber + 1);
		}
	}

	public void selectPreviousBank() {
		if (canSelectPreviousBank()) {
			int orderNumber = selectedBank.getOrderNumber();
			selectBankWithOrderNumber(orderNumber - 1);
		}
	}

	private void selectBankWithOrderNumber(int i) {
		for (MapsControl c : selectedBank.getParent().getChildren()) {
			if (c instanceof Bank) {
				Bank b = (Bank) c;
				if (b.getOrderNumber() == i) {
					setSelectedBank(b);
					return;
				}
			}
		}
	}

	public Preset getOriginalPreset() {
		return originPreset;
	}

	public Bank getSelectedBank() {
		return selectedBank;
	}

	public void updateUI() {
		NonMaps.get().getNonLinearWorld().invalidate(NonLinearWorld.INVALIDATION_FLAG_UI_CHANGED);
		NonMaps.get().getNonLinearWorld().getViewport().getOverlay().getBelt().getPresetLayout().getBankControl()
				.update();
	}

	public boolean isOriginalPreset(Preset mapsPreset) {
		return mapsPreset == originPreset;
	}
}
