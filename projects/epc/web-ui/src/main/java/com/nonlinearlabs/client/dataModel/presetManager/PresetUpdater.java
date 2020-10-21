package com.nonlinearlabs.client.dataModel.presetManager;

import com.google.gwt.xml.client.Node;
import com.nonlinearlabs.client.dataModel.Updater;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.SoundType;

public class PresetUpdater extends Updater {

	protected Preset target;

	public PresetUpdater(Node n, Preset preset) {
		super(n);
		target = preset;
	}

	public void doUpdate() {
		target.name.setValue(getAttributeValue(root, "name"));
		target.uuid.setValue(getAttributeValue(root, "uuid"));
		target.partNameVGI.setValue(getAttributeValue(root, "part-I-name"));
		target.partNameVGII.setValue(getAttributeValue(root, "part-II-name"));

		try {
			target.type.setValue(SoundType.valueOf(getAttributeValue(root, "type")));
		} catch (Exception e) {
			target.type.setValue(SoundType.Single);
		}

		processChildrenElements(root, "attribute", v -> {
			var key = getAttributeValue(v, "key");
			var value = getText(v);

			if (key == "Comment")
				target.comment.setValue(value);
			else if (key == "DeviceName")
				target.deviceName.setValue(value);
			else if (key == "SoftwareVersion")
				target.softwareVersion.setValue(value);
			else if (key == "StoreTime")
				target.storeTime.setValue(value);
			else if (key == "StoreCounter")
				target.storeCounter.setValue(Integer.valueOf(value));
			else if (key == "color")
				target.color.setValue(Preset.Color.valueOf(value));
		});
	}

	protected void handlePresetParameters(Node xml, Preset preset) {
	}
}
