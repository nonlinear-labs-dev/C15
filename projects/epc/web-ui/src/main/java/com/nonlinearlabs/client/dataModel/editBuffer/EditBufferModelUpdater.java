package com.nonlinearlabs.client.dataModel.editBuffer;

import com.google.gwt.core.client.GWT;
import com.google.gwt.xml.client.Node;
import com.nonlinearlabs.client.dataModel.Updater;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.SoundType;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.VoiceGroup;
import com.nonlinearlabs.client.dataModel.editBuffer.ModulateableParameterModel.ModSource;
import com.nonlinearlabs.client.dataModel.setup.SetupModel;

public class EditBufferModelUpdater extends Updater {

	public EditBufferModelUpdater(Node editBufferNode) {
		super(editBufferNode);
	}

	@Override
	public void doUpdate() {
		if (root != null) {
			String selParamStr = getAttributeValue(root, "selected-parameter");
			ParameterId selParam = new ParameterId(selParamStr);
			EditBufferModel.get().selectedParameter.setValue(selParam.getNumber());
			String loadedPreset = getAttributeValue(root, "loaded-preset");
			GWT.log("Loaded Preset:" + loadedPreset);
			EditBufferModel.get().loadedPreset.setValue(loadedPreset);
			String loadedPresetName = getAttributeValue(root, "loaded-presets-name");
			EditBufferModel.get().loadedPresetName.setValue(loadedPresetName);
			String loadedPresetBankName = getAttributeValue(root, "loaded-presets-bank-name");
			EditBufferModel.get().loadedPresetBankName.setValue(loadedPresetBankName);
			String isZombie = getAttributeValue(root, "preset-is-zombie");
			EditBufferModel.get().isZombie.setValue(Boolean.valueOf(isZombie));
			String isModified = getAttributeValue(root, "is-modified");
			EditBufferModel.get().isModified.setValue(Boolean.valueOf(isModified));
			String changed = getAttributeValue(root, "changed");
			EditBufferModel.get().isChanged.setValue(Boolean.valueOf(changed));
			String soundType = getAttributeValue(root, "editbuffer-type");
			EditBufferModel.get().soundType.setValue(EditBufferModel.SoundType.valueOf(soundType));

			if (EditBufferModel.SoundType.valueOf(soundType) == SoundType.Single)
				EditBufferModel.get().voiceGroup.setValue(VoiceGroup.I);

			String vgIName = getAttributeValue(root, "vg-I-name");
			EditBufferModel.get().loadedPresetInVG1.setValue(vgIName);

			String vgIIName = getAttributeValue(root, "vg-II-name");
			EditBufferModel.get().loadedPresetInVG2.setValue(vgIIName);

			String vgINameSuffixed = getAttributeValue(root, "vg-I-name-with-suffix");
			EditBufferModel.get().loadedPresetInVG1WithSuffix.setValue(vgINameSuffixed);

			String vgIINameSuffixed = getAttributeValue(root, "vg-II-name-with-suffix");
			EditBufferModel.get().loadedPresetInVG2WithSuffix.setValue(vgIINameSuffixed);

			String srcUUIDI = getAttributeValue(root, "origin-I");
			EditBufferModel.get().sourceUUIDI.setValue(srcUUIDI);

			String srcUUIDII = getAttributeValue(root, "origin-II");
			EditBufferModel.get().sourceUUIDII.setValue(srcUUIDII);

			String srcVGI = getAttributeValue(root, "origin-I-vg");
			if (!srcVGI.isEmpty())
				EditBufferModel.get().sourceVGI.setValue(VoiceGroup.valueOf(srcVGI));

			String srcVGII = getAttributeValue(root, "origin-II-vg");
			if (!srcVGII.isEmpty())
				EditBufferModel.get().sourceVGII.setValue(VoiceGroup.valueOf(srcVGII));

			processChangedChildrenElements(root, "recall-data", c -> processOriginal(c));
		}

		processChangedChildrenElements(root, "global-parameters", child -> {
			processChangedChildrenElements(child, "parameter-group", group -> processGroup(group, VoiceGroup.Global));
		});

		processChangedChildrenElements(root, "voice-group-I-parameters", child -> {
			processChangedChildrenElements(child, "parameter-group", group -> processGroup(group, VoiceGroup.I));
		});

		processChangedChildrenElements(root, "voice-group-II-parameters", child -> {
			processChangedChildrenElements(child, "parameter-group", group -> processGroup(group, VoiceGroup.II));
		});
	}

	private void processOriginal(Node c) {
		processChildrenElements(c, "param", p -> processOriginalParameter(p));
	}

	private void processGroup(Node c, VoiceGroup vg) {
		String groupIdStr = getAttributeValue(c, "id");
		GroupId groupId = new GroupId(groupIdStr);
		ParameterGroupModel target = EditBufferModel.get().getGroup(groupId);
		ParameterGroupModelUpdater updater = new ParameterGroupModelUpdater(c, target, groupId);
		updater.doUpdate();
	}

	private void processOriginalParameter(Node param) {
		if (param != null) {
			String id = getAttributeValue(param, "id");
			String val = getAttributeValue(param, "value");
			String modSrc = getAttributeValue(param, "mod-src");
			String modAmt = getAttributeValue(param, "mod-amt");

			if (!id.isEmpty()) {
				BasicParameterModel bpm = EditBufferModel.get().getParameter(new ParameterId(id));
				if (!val.isEmpty())
					bpm.originalValue.setValue(Double.valueOf(val));

				if (bpm instanceof ModulateableParameterModel) {
					ModulateableParameterModel modP = ((ModulateableParameterModel) bpm);
					if (!modAmt.isEmpty())
						modP.ogModAmount.setValue(Double.valueOf(modAmt));
					if (!modSrc.isEmpty())
						modP.ogModSource.setValue(ModSource.values()[Integer.valueOf(modSrc)]);
				}
			}
		}
	}
}
