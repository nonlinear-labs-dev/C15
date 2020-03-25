package com.nonlinearlabs.client.dataModel.editBuffer;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import com.nonlinearlabs.client.dataModel.BooleanDataModelEntity;
import com.nonlinearlabs.client.dataModel.DateDataModelEntity;
import com.nonlinearlabs.client.dataModel.EnumDataModelEntity;
import com.nonlinearlabs.client.dataModel.IntegerDataModelEntity;
import com.nonlinearlabs.client.dataModel.Notifier;
import com.nonlinearlabs.client.dataModel.StringDataModelEntity;
import com.nonlinearlabs.client.dataModel.editBuffer.ModulateableParameterModel.ModSource;

public class EditBufferModel extends Notifier<EditBufferModel> {
	private static EditBufferModel theModel = new EditBufferModel();

	public static EditBufferModel get() {
		return theModel;
	}

	public static enum Color {
		green, blue, yellow, orange, purple, red, none;
	}

	public static enum SoundType {
		Single, Split, Layer
	}

	public static enum VoiceGroup {
		I, II, Global
	}

	public static class ByVoiceGroup {
		public HashMap<String, ParameterGroupModel> parameterGroups = new HashMap<String, ParameterGroupModel>();
		public HashMap<Integer, BasicParameterModel> parameters = new HashMap<Integer, BasicParameterModel>();
		public ArrayList<ModulateableParameterModel> modulateableParametersCache = new ArrayList<ModulateableParameterModel>();
	}

	public ByVoiceGroup[] byVoiceGroup = { new ByVoiceGroup(), new ByVoiceGroup(), new ByVoiceGroup() };

	public IntegerDataModelEntity selectedParameter = new IntegerDataModelEntity();
	public StringDataModelEntity loadedPreset = new StringDataModelEntity();
	public StringDataModelEntity loadedPresetName = new StringDataModelEntity();
	public StringDataModelEntity loadedPresetBankName = new StringDataModelEntity();
	public BooleanDataModelEntity isZombie = new BooleanDataModelEntity();
	public BooleanDataModelEntity isModified = new BooleanDataModelEntity();
	public BooleanDataModelEntity isChanged = new BooleanDataModelEntity();

	public EnumDataModelEntity<Color> color = new EnumDataModelEntity<Color>(Color.class, Color.none);
	public EnumDataModelEntity<SoundType> soundType = new EnumDataModelEntity<SoundType>(SoundType.class,
			SoundType.Single);
	public EnumDataModelEntity<VoiceGroup> voiceGroup = new EnumDataModelEntity<VoiceGroup>(VoiceGroup.class,
			VoiceGroup.I);
	public StringDataModelEntity comment = new StringDataModelEntity();
	public StringDataModelEntity deviceName = new StringDataModelEntity();
	public DateDataModelEntity storeTime = new DateDataModelEntity();

	public StringDataModelEntity loadedPresetInVG1 = new StringDataModelEntity("");
	public StringDataModelEntity loadedPresetInVG1WithSuffix = new StringDataModelEntity("");
	public StringDataModelEntity loadedPresetInVG2 = new StringDataModelEntity("");
	public StringDataModelEntity loadedPresetInVG2WithSuffix = new StringDataModelEntity("");

	private EditBufferModel() {
		ParameterFactory.assertSorted();

		for (String groupdId : ParameterFactory.getParameterGroups()) {
			if (ParameterFactory.isGlobalParameterGroup(groupdId)) {
				byVoiceGroup[VoiceGroup.Global.ordinal()].parameterGroups.put(groupdId, new ParameterGroupModel());
			} else {
				byVoiceGroup[VoiceGroup.I.ordinal()].parameterGroups.put(groupdId, new ParameterGroupModel());
				byVoiceGroup[VoiceGroup.II.ordinal()].parameterGroups.put(groupdId, new ParameterGroupModel());
			}
		}

		for (int number : ParameterFactory.getAllParameters()) {
			if (ParameterFactory.isGlobalParameter(number)) {
				addParameter(new ParameterId(number, VoiceGroup.Global));
			} else {
				addParameter(new ParameterId(number, VoiceGroup.I));
				addParameter(new ParameterId(number, VoiceGroup.II));
			}
		}
	}

	public ParameterGroupModel getGroup(GroupId id) {
		return byVoiceGroup[id.getVoiceGroup().ordinal()].parameterGroups.get(id.getName());
	}

	public BasicParameterModel getParameter(ParameterId id) {
		BasicParameterModel r = byVoiceGroup[id.getVoiceGroup().ordinal()].parameters.get(id.getNumber());

		if (r == null)
			throw new IllegalArgumentException();

		return r;
	}

	public BasicParameterModel getSelectedParameter() {
		int paramID = selectedParameter.getValue();
		BasicParameterModel global = byVoiceGroup[VoiceGroup.Global.ordinal()].parameters.get(paramID);
		if (global != null)
			return global;

		return byVoiceGroup[voiceGroup.getValue().ordinal()].parameters.get(paramID);
	}

	private void addParameter(ParameterId id) {
		BasicParameterModel p = ParameterFactory.create(id);

		if (p != null) {
			byVoiceGroup[id.getVoiceGroup().ordinal()].parameters.put(id.getNumber(), p);

			if (p instanceof ModulateableParameterModel)
				byVoiceGroup[id.getVoiceGroup().ordinal()].modulateableParametersCache
						.add((ModulateableParameterModel) p);
		}
	}

	public String getPresetNameOfVoiceGroup(VoiceGroup group) {
		switch (group) {
		case I:
		case Global:
			return loadedPresetInVG1.getValue();

		case II:
			return loadedPresetInVG2.getValue();
		}
		return "";
	}

	public String getPresetNameOfVoiceGroupWithSuffix(VoiceGroup group) {
		switch(group) {
			case I:
			case Global:
			return loadedPresetInVG1WithSuffix.getValue();
			case II:
			return loadedPresetInVG2WithSuffix.getValue();
		}
		return "";
	}

	public MacroControlParameterModel getParameter(ModSource value, VoiceGroup vg) {
		return (MacroControlParameterModel) getParameter(value.toParameterId());
	}

	public List<ModulateableParameterModel> getAllModulateableParameters() {
		return byVoiceGroup[voiceGroup.getValue().ordinal()].modulateableParametersCache;
	}

	public ParameterGroupModel getAnyGroup(String id) {
		ParameterGroupModel g = byVoiceGroup[VoiceGroup.Global.ordinal()].parameterGroups.get(id);
		if (g != null)
			return g;
		return byVoiceGroup[VoiceGroup.I.ordinal()].parameterGroups.get(id);
	}

	public BasicParameterModel getAnyParameter(int id) {
		BasicParameterModel p = byVoiceGroup[VoiceGroup.Global.ordinal()].parameters.get(id);

		if (p != null)
			return p;

		return byVoiceGroup[VoiceGroup.I.ordinal()].parameters.get(id);
	}

	@Override
	public EditBufferModel getValue() {
		return this;
	}

}