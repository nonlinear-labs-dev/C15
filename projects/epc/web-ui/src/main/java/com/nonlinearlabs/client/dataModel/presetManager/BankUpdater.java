package com.nonlinearlabs.client.dataModel.presetManager;

import java.util.ArrayList;

import com.google.gwt.xml.client.Node;
import com.nonlinearlabs.client.dataModel.Updater;

public class BankUpdater extends Updater {

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

		bank.selectedPreset.setValue(getAttributeValue(root, "selected-preset"));
		bank.x.setValue(Double.parseDouble(getAttributeValue(root, "x")));
		bank.y.setValue(Double.parseDouble(getAttributeValue(root, "y")));
		bank.orderNumber.setValue(Integer.parseInt(getAttributeValue(root, "order-number")));

		Presets.get().preUpdate(bank.uuid.getValue());
		ArrayList<String> existingPresets = new ArrayList<String>();
		super.processChildrenElements(root, "preset", p -> updatePreset(existingPresets, p));
		bank.presets.setValue(existingPresets);

		int presetNumber = 1;
		for (String uuid : existingPresets) {
			Preset preset = Presets.get().find(uuid);
			preset.number.setValue(presetNumber++);
		}
	}

	private void updatePreset(ArrayList<String> existingPresets, Node presetNode) {
		String uuid = getAttributeValue(presetNode, "uuid");
		existingPresets.add(uuid);
		boolean dirty = didChange(presetNode);
		Preset preset = Presets.get().find(uuid);

		if (preset == null) {
			preset = Presets.get().put(uuid);
			dirty = true;
		}

		preset.bankUuid.setValue(bank.uuid.getValue());
		preset.revive();

		if (dirty) {
			PresetUpdater updater = new PresetUpdater(presetNode, preset);
			updater.doUpdate();
		}
	}

}
