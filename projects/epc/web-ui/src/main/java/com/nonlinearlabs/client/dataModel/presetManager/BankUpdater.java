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
		bank.orderNumber.setValue(Integer.parseInt(getAttributeValue(root, "order-number")));

		bank.x.setValue(Double.parseDouble(getAttributeValue(root, "x")));
		bank.y.setValue(Double.parseDouble(getAttributeValue(root, "y")));
		bank.attachedToBank.setValue(getChildText(root, "attached-to"));
		bank.attachDirection.setValue(getChildText(root, "attached-direction"));
		bank.dateOfLastChange.setValue(getChildText(root, "date-of-last-change"));
		bank.importExportState.setValue(getChildText(root, "state"));

		processChildrenElements(root, "attribute", v -> {
			var key = getAttributeValue(v, "key");
			var value = getText(v);

			if (key == "Comment")
				bank.comment.setValue(value);
			else if (key == "Date of Export File")
				bank.dateOfExportFile.setValue(value);
			else if (key == "Date of Import File")
				bank.dateOfImportFile.setValue(value);
			else if (key == "Name of Export File")
				bank.nameOfExportFile.setValue(value);
			else if (key == "Name of Import File")
				bank.nameOfImportFile.setValue(value);
		});

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
