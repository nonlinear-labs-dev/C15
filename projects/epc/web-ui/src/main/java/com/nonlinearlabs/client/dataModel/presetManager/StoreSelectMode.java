package com.nonlinearlabs.client.dataModel.presetManager;

import com.nonlinearlabs.client.NonMaps;

public class StoreSelectMode implements CustomPresetSelection {
	private String selectedPreset;
	private String selectedBank;
	private final String originPreset = "";

	public StoreSelectMode() {

		// originPreset = parent.findLoadedPreset();
		// selectedPreset = m_parent.getSelectedPreset();
		// selectedBank = m_parent.findBank(m_parent.getSelectedBank());
		NonMaps.get().getServerProxy().setSetting("storemode", "on");
	}

	public void setSelectedPreset(String p) {
		selectedPreset = p;
		// selectedBank = p != null ? p.getParent() : null;
	}

	public void setSelectedBank(String b) {
		selectedBank = b;
		// selectedPreset =
		// b.getPresetList().findPreset(b.getPresetList().getSelectedPreset());
	}

	public String getSelectedPreset() {
		return selectedPreset;
	}

	public boolean canNext() {
		// if (selectedPreset != null)
		// return selectedPreset.getNumber() <
		// selectedPreset.getParent().getPresetList().getPresetCount();

		return false;
	}

	public boolean canPrev() {
		// if (selectedPreset != null)
		// return selectedPreset.getNumber() > 1;

		return false;
	}

	public void selectNextPreset() {
		if (canNext()) {
			// Bank b = selectedPreset.getParent();
			// int idx = selectedPreset.getNumber() - 1;
			// Preset next = b.getPreset(idx + 1);
			// setSelectedPreset(next);
		}
	}

	public void selectPreviousPreset() {
		if (canPrev()) {
			// Bank b = selectedPreset.getParent();
			//// int idx = selectedPreset.getNumber() - 1;
			// Preset next = b.getPreset(idx - 1);
			// setSelectedPreset(next);
		}
	}

	public boolean canSelectPreviousBank() {
		// if (selectedBank != null) {
		// return
		// selectedBank.getParent().canSelectBankWithOrdernumberOffset(selectedBank,
		// -1);
		// }
		return false;
	}

	public boolean canSelectNextBank() {
		if (selectedBank != null) {
			// return
			// selectedBank.getParent().canSelectBankWithOrdernumberOffset(selectedBank, 1);
		}
		return false;
	}

	public void selectNextBank() {
		if (canSelectNextBank()) {
			// int orderNumber = selectedBank.getOrderNumber();
			// selectBankWithOrderNumber(orderNumber + 1);
		}
	}

	public void selectPreviousBank() {
		if (canSelectPreviousBank()) {
			// int orderNumber = selectedBank.getOrderNumber();
			// selectBankWithOrderNumber(orderNumber - 1);
		}
	}

	private void selectBankWithOrderNumber(int i) {
		// for (MapsControl c : selectedBank.getParent().getChildren()) {
		// if (c instanceof Bank) {
		// Bank b = (Bank) c;
		// if (b.getOrderNumber() == i) {
		// setSelectedBank(b);
		// return;
		// }
		// }
		// }
	}

	public String getOriginalPreset() {
		return originPreset;
	}

	public String getSelectedBank() {
		return selectedBank;
	}

	public boolean isOriginalPreset(String mapsPreset) {
		return mapsPreset == originPreset;
	}
}
