package com.nonlinearlabs.client.dataModel.presetManager;

import java.util.ArrayList;

import com.google.gwt.xml.client.Node;
import com.nonlinearlabs.client.dataModel.Updater;

public class PresetManagerUpdater extends Updater {

	protected PresetManagerModel target;

	public PresetManagerUpdater(Node c, PresetManagerModel pm) {
		super(c);
		target = pm;
	}

	public void doUpdate() {
		if (didChange(root)) {
			processChangedChildrenElements(root, "banks", t -> updateBanks(t));
			Banks.get().postUpdate();
			Presets.get().postUpdate();
		}
	}

	public void updateBankPositions() {
		target.positions.setValue(Banks.get().updatePositions());
	}

	private void updateBanks(Node banks) {
		if (didChange(banks)) {
			var banksDB = Banks.get();
			banksDB.preUpdate();
			String midiSelectedBank = banks.getAttributes().getNamedItem("selected-midi-bank").getNodeValue();
			ArrayList<String> existingBanks = new ArrayList<String>();
			processChildrenElements(banks, "preset-bank", t -> updateBank(existingBanks, t));
			existingBanks
					.sort((a, b) -> banksDB.find(a).orderNumber.getValue() - banksDB.find(b).orderNumber.getValue());
			target.banks.setValue(existingBanks);
			target.selectedBank.setValue(getAttributeValue(banks, "selected-bank"));
		}
	}

	private void updateBank(ArrayList<String> existingBanks, Node bankNode, String midiUuid) {
		if (didChange(bank)) {
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
				BankUpdater updater = new BankUpdater(bank, b, midiUuid);
				updater.doUpdate();
			}
		}
	}
}
