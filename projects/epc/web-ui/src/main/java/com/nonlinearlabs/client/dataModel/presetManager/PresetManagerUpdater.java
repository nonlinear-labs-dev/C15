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

	private void updateBanks(PresetManagerModel pm, Node banks) {
		String midiSelectedBank = banks.getAttributes().getNamedItem("selected-midi-bank").getNodeValue();
		BankMapDataModelEntity existingBanksEntity = pm.getBanks();
		Map<String, Bank> existingBanks = new HashMap<String, Bank>(existingBanksEntity.getValue());
		existingBanks.forEach((uuid, bank) -> bank.setDoomed());
		processChildrenElements(banks, "preset-bank", t -> updateBank(existingBanks, t, midiSelectedBank));
		existingBanks.entrySet().removeIf(e -> e.getValue().isDoomed());
		existingBanksEntity.setValue(existingBanks);
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
