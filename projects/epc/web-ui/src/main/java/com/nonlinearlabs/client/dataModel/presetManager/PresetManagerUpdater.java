package com.nonlinearlabs.client.dataModel.presetManager;

import java.util.ArrayList;

import com.google.gwt.xml.client.Node;
import com.nonlinearlabs.client.dataModel.Updater;

public class PresetManagerUpdater extends Updater {

	protected PresetManagerModel target;
	private Banks banksDB = Banks.get();

	public PresetManagerUpdater(Node c, PresetManagerModel pm) {
		super(c);
		target = pm;
	}

	public void doUpdate() {
		if (didChange(root)) {
			banksDB.preUpdate();
			target.fileVersion.setValue(Integer.parseInt(getAttributeValue(root, "file-version")));
			processChangedChildrenElements(root, "banks", t -> updateBanks(t));
			banksDB.postUpdate();
			Presets.get().postUpdate();
		}
	}

	private void updateBanks(Node banks) {
		ArrayList<String> existingBanks = new ArrayList<String>();
		processChildrenElements(banks, "preset-bank", t -> updateBank(existingBanks, t));
		existingBanks.sort((a, b) -> banksDB.find(a).orderNumber.getValue() - banksDB.find(b).orderNumber.getValue());
		target.banks.setValue(existingBanks);
		target.selectedBank.setValue(getAttributeValue(banks, "selected-bank"));
		target.midiBank.setValue(getAttributeValue(banks, "selected-midi-bank"));
	}

	private void updateBank(ArrayList<String> existingBanks, Node bankNode) {
		String uuid = getAttributeValue(bankNode, "uuid");
		existingBanks.add(uuid);
		boolean dirty = didChange(bankNode);
		Bank bank = Banks.get().find(uuid);

		if (bank == null) {
			bank = Banks.get().put(uuid);
			dirty = true;
		}

		bank.revive();

		if (dirty) {
			BankUpdater updater = new BankUpdater(bankNode, bank);
			updater.doUpdate();
		}
	}

	public void updateBankPositions() {
		target.positions.setValue(Banks.get().updatePositions(), true);
	}

}
