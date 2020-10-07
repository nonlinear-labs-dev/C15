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
			Banks.get().preUpdate();
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
			String midiSelectedBank = banks.getAttributes().getNamedItem("selected-midi-bank").getNodeValue();
			GWT.log("updating Banks with midi uuid: " + midiSelectedBank);
			BankMapDataModelEntity existingBanksEntity = pm.getBanks();
			Map<String, Bank> existingBanks = new HashMap<String, Bank>(existingBanksEntity.getValue());
			existingBanks.forEach((uuid, bank) -> bank.setDoomed());
			processChildrenElements(banks, "preset-bank", t -> updateBank(existingBanks, t, midiSelectedBank));
			existingBanks.entrySet().removeIf(e -> e.getValue().isDoomed());
			existingBanksEntity.setValue(existingBanks);
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
