package com.nonlinearlabs.client.dataModel.presetManager;

import java.util.HashMap;
import java.util.Map;

import com.google.gwt.xml.client.Node;
import com.nonlinearlabs.client.dataModel.Updater;

public class PresetManagerUpdater extends Updater {

	protected PresetManagerModel target;

	public PresetManagerUpdater(Node c, PresetManagerModel pm) {
		super(c);
		target = pm;
	}

	public void doUpdate() {
		if (didChange(root))
			processChangedChildrenElements(root, "banks", t -> updateBanks(target, t));
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

	private void updateBank(Map<String, Bank> existingBanks, Node bank, String midiUuid) {
		if (didChange(bank)) {
			String uuid = getAttributeValue(bank, "uuid");
			Bank b = existingBanks.get(uuid);
			boolean dirty = didChange(bank);

			if (b == null) {
				b = new Bank();
				existingBanks.put(uuid, b);
				dirty = true;
			}

			b.revive();

			if (dirty) {
				BankUpdater updater = new BankUpdater(bank, b, midiUuid);
				updater.doUpdate();
			}
		}
	}
}
