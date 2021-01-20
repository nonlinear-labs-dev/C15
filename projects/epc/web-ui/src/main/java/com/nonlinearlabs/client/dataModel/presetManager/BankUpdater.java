package com.nonlinearlabs.client.dataModel.presetManager;

import java.util.ArrayList;

import com.google.gwt.xml.client.Node;
import com.nonlinearlabs.client.dataModel.Updater;

public class BankUpdater extends Updater {

	private int pos = 0;
	private Bank bank;
	private String selectedMidiUuid;

	public BankUpdater(Node c, Bank target, String midiUuid) {
		super(c);
		bank = target;
		selectedMidiUuid = midiUuid;
	}

	@Override
	public void doUpdate() {
		bank.name.setValue(getAttributeValue(root, "name"));
		bank.uuid.setValue(getAttributeValue(root, "uuid"));
		bank.isMidiBank.setValue(selectedMidiUuid.equals(bank.uuid.getValue()));

		if (bank.isMidiBank.isTrue()) {
			bank.name.setValue(bank.name.getValue() + " ^");
		}

		ArrayList<Preset> existingPresets = new ArrayList<Preset>(bank.getPresets().getValue());
		existingPresets.forEach(p -> p.setDoomed());
		super.processChildrenElements(root, "preset", p -> updatePreset(existingPresets, p));
		existingPresets.removeIf(p -> p.isDoomed());
		bank.getPresets().setValue(existingPresets);
	}

	private void updatePreset(ArrayList<Preset> existingPresets, Node p) {
		if (existingPresets.size() <= pos)
			existingPresets.add(new Preset());

		String uuid = getAttributeValue(p, "uuid");
		Preset preset = existingPresets.get(pos);
		preset.revive();

		if (didChange(p) || !preset.uuid.getValue().equals(uuid)) {
			PresetUpdater updater = new PresetUpdater(p, preset);
			updater.doUpdate();
		}
	}

}
