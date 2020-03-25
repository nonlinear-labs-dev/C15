package com.nonlinearlabs.client.useCases;

import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.dataModel.editBuffer.BasicParameterModel;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.SoundType;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.VoiceGroup;
import com.nonlinearlabs.client.dataModel.editBuffer.MacroControlParameterModel;
import com.nonlinearlabs.client.dataModel.editBuffer.ModulateableParameterModel;
import com.nonlinearlabs.client.dataModel.editBuffer.ModulateableParameterModel.ModSource;
import com.nonlinearlabs.client.dataModel.editBuffer.ModulationRouterParameterModel;
import com.nonlinearlabs.client.dataModel.editBuffer.ParameterFactory;
import com.nonlinearlabs.client.dataModel.editBuffer.ParameterId;
import com.nonlinearlabs.client.dataModel.editBuffer.PhysicalControlParameterModel;
import com.nonlinearlabs.client.dataModel.editBuffer.RibbonParameterModel;
import com.nonlinearlabs.client.dataModel.setup.SetupModel;
import com.nonlinearlabs.client.dataModel.setup.SetupModel.LoadMode;
import com.nonlinearlabs.client.tools.NLMath;

public class EditBufferUseCases {
	private static EditBufferUseCases theInstance = new EditBufferUseCases();
	private AnimationManager animationManager = new AnimationManager();

	public static EditBufferUseCases get() {
		return theInstance;
	}

	public void setParameterValue(ParameterId id, double newValue, boolean oracle) {
		setParameterValue(id, newValue, oracle, true);
	}

	private void setParameterValue(ParameterId id, double newValue, boolean oracle, boolean setAnimationTimeout) {
		BasicParameterModel p = EditBufferModel.get().getParameter(id);
		double oldQ = p.value.getQuantizedAndClipped(true);
		p.value.value.setValue(newValue);
		double newQ = p.value.getQuantizedAndClipped(true);
		double diff = newQ - oldQ;

		if (p instanceof PhysicalControlParameterModel) {
			PhysicalControlParameterModel m = (PhysicalControlParameterModel) p;
			applyPhysicalControlModulation(m, diff);

			if (m.isReturning()) {
				if (setAnimationTimeout)
					animationManager.startDelayedAnimation(m, 2000);
				else
					animationManager.cancelAnimation(m);
			}
		}

		if (p instanceof MacroControlParameterModel)
			applyModulationToModulateableParameters(id, diff);

		NonMaps.get().getServerProxy().setParameter(id, newValue, oracle);
	}

	private void startReturningAnimation(PhysicalControlParameterModel m) {
		animationManager.startReturnAnimation(m);
	}

	private void handleBidirectionalRibbonBinding(MacroControlParameterModel m) {
		for (ParameterId routerId : m.getRouterIDs()) {
			ModulationRouterParameterModel r = this.<ModulationRouterParameterModel>findParameter(routerId);

			if (r.value.getQuantized(true) > 0.0) {
				ParameterId physicalControlID = r.getAssociatedPhysicalControlID();

				int ribbon1 = 284;
				int ribbon2 = 289;

				if (physicalControlID.getNumber() == ribbon1 || physicalControlID.getNumber() == ribbon2) {
					RibbonParameterModel ribbon = this.<RibbonParameterModel>findParameter(physicalControlID);

					if (!ribbon.isReturning()) {
						ribbon.value.value.setValue(m.value.getQuantizedAndClipped(true));
					}
				}
			}
		}
	}

	private <T> T findParameter(ParameterId id) {
		return (T) EditBufferModel.get().getParameter(id);
	}

	private void applyPhysicalControlModulation(PhysicalControlParameterModel p, double diff) {
		if (Math.abs(diff) > 0.0) {

			for (ParameterId router : p.getAssociatedModulationRouters()) {
				ModulationRouterParameterModel routerParameter = (ModulationRouterParameterModel) EditBufferModel.get()
						.getParameter(router);

				if (p.isReturning())
					applyReturningModulation(routerParameter, diff);
				else if (routerParameter.value.value.getValue() != 0) {
					applyDirectModulation(routerParameter, p.value.getQuantizedAndClipped(true));
				}
			}
		}
	}

	private void applyReturningModulation(ModulationRouterParameterModel routerParameter, double diff) {
		double amount = routerParameter.value.getQuantizedAndClipped(true);

		if (amount != 0.0) {
			ParameterId mc = routerParameter.getAssociatedMacroControlID();
			applyModulation(mc, diff * amount);
		}
	}

	private void applyModulation(ParameterId macroControlID, double delta) {
		BasicParameterModel macroControl = EditBufferModel.get().getParameter(macroControlID);

		double oldQ = macroControl.value.getQuantizedAndClipped(true);
		double v = macroControl.value.value.getValue();
		macroControl.value.value.setValue(v + delta);
		double newQ = macroControl.value.getQuantizedAndClipped(true);
		applyModulationToModulateableParameters(macroControlID, newQ - oldQ);
	}

	private void applyModulationToModulateableParameters(ParameterId macroControlID, double d) {
		if (d != 0) {
			ModSource m = ModSource.fromParameterId(macroControlID);
			for (ModulateableParameterModel t : EditBufferModel.get().getAllModulateableParameters()) {
				if (t.modSource.getValue() == m) {
					double amount = t.modAmount.getQuantizedAndClipped(true);
					if (t.value.metaData.bipolar.getBool())
						amount *= 2;
					t.value.value.setValue(t.value.value.getValue() + d * amount);
				}
			}

			MacroControlParameterModel mc = this.<MacroControlParameterModel>findParameter(macroControlID);
			handleBidirectionalRibbonBinding(mc);
		}
	}

	private void applyDirectModulation(ModulationRouterParameterModel routerParameter, double newValue) {
		MacroControlParameterModel m = (MacroControlParameterModel) EditBufferModel.get()
				.getParameter(routerParameter.getAssociatedMacroControlID());
		double oldQ = m.value.getQuantizedAndClipped(true);
		m.value.value.setValue(newValue);
		double newQ = m.value.getQuantizedAndClipped(true);
		applyModulationToModulateableParameters(m.id, newQ - oldQ);
	}

	public void selectParameter(int paramNumber) {
		ParameterId id = toParamId(paramNumber);
		if (EditBufferModel.get().selectedParameter.setValue(id.getNumber()))
			NonMaps.get().getServerProxy().selectParameter(id);
	}

	public void incParameter(int paramNumber, boolean fine) {
		incDecParameter(paramNumber, fine, 1);
	}

	public void incParameter(ParameterId id, boolean fine) {
		incDecParameter(id, fine, 1);
	}

	public void decParameter(int paramNumber, boolean fine) {
		incDecParameter(paramNumber, fine, -1);
	}

	public void decParameter(ParameterId id, boolean fine) {
		incDecParameter(id, fine, -1);
	}

	private void incDecParameter(int paramNumber, boolean fine, int inc) {
		ParameterId id = toParamId(paramNumber);
		incDecParameter(id, fine, inc);
	}

	private void incDecParameter(ParameterId id, boolean fine, int inc) {
		BasicParameterModel p = EditBufferModel.get().getParameter(id);
		double v = p.getIncDecValue(fine, inc);
		setParameterValue(id, v, true);
	}

	public ParameterId toParamId(int paramNumber) {
		return new ParameterId(paramNumber, getVoiceGroupFor(paramNumber));
	}

	public void initializeSound() {
		NonMaps.get().getServerProxy().initSound();
	}

	public void randomizeSound() {
		NonMaps.get().getServerProxy().randomizeSound();
	}

	public void convertToSingleSound() {
		NonMaps.get().getServerProxy().convertToSingle(EditBufferModel.get().voiceGroup.getValue());
		EditBufferModel.get().soundType.setValue(SoundType.Single);
	}

	public void convertToSplitSound() {
		NonMaps.get().getServerProxy().convertToSplit();
		EditBufferModel.get().soundType.setValue(SoundType.Split);
	}

	public void convertToLayerSound() {
		NonMaps.get().getServerProxy().convertToLayer();
		EditBufferModel.get().soundType.setValue(SoundType.Layer);
	}

	public void setToDefault(int paramNumber) {
		ParameterId id = toParamId(paramNumber);
		BasicParameterModel p = EditBufferModel.get().getParameter(id);
		double v = p.value.metaData.defaultValue.getValue();
		setParameterValue(id, v, true);
	}

	public void toggleBoolean(int paramNumber) {
		ParameterId id = toParamId(paramNumber);
		BasicParameterModel p = EditBufferModel.get().getParameter(id);

		if (p.value.getQuantizedAndClipped(true) != 0.0)
			setParameterValue(id, 0, true);
		else
			setParameterValue(id, 1, true);
	}

	public void decModulationAmount(ParameterId id, boolean fine) {
		incDecModulationAmount(id, fine, -1);
	}

	public void incModulationAmount(ParameterId id, boolean fine) {
		incDecModulationAmount(id, fine, 1);
	}

	public void resetModulationAmount(ParameterId id) {
		setModulationAmount(id, 0, false);
	}

	private void incDecModulationAmount(ParameterId id, boolean fine, int inc) {
		ModulateableParameterModel p = (ModulateableParameterModel) EditBufferModel.get().getParameter(id);
		double v = p.modAmount.getIncDecValue(fine, inc);
		setModulationAmount(id, v, true);
	}

	public void setModulationAmount(ParameterId id, double newValue, boolean fine) {
		ModulateableParameterModel p = (ModulateableParameterModel) EditBufferModel.get().getParameter(id);
		double oldValue = p.modAmount.getQuantizedAndClipped(fine);
		p.modAmount.value.setValue(newValue);
		newValue = p.modAmount.getQuantizedAndClipped(fine);
		double diff = newValue - oldValue;

		if (diff != 0)
			NonMaps.get().getServerProxy().setModulationAmount(newValue);
	}

	private void setModulationUpperBound(ParameterId id, double newAmount, boolean fine) {
		ModulateableParameterModel p = (ModulateableParameterModel) EditBufferModel.get().getParameter(id);
		ParameterId mcId = new ParameterId(p.modSource.getValue());
		MacroControlParameterModel mc = (MacroControlParameterModel) EditBufferModel.get().getParameter(mcId);

		double factor = p.value.metaData.bipolar.getBool() ? 2 : 1;
		double oldAmount = p.modAmount.getClippedValue();
		double oldValue = p.value.getClippedValue();
		double mcValue = mc.value.getClippedValue();
		double oldLowerBound = oldValue - (factor * oldAmount) * mcValue;

		if (p.value.metaData.bipolar.getBool()) {
			double oldLowerCP = NLMath.clamp((oldLowerBound + 1) / 2, 0, 1.0);
			newAmount = NLMath.clamp(newAmount, -oldLowerCP, 1.0 - oldLowerCP);
		} else {
			double oldLowerCP = NLMath.clamp(oldLowerBound, 0, 1.0);
			newAmount = NLMath.clamp(newAmount, -oldLowerCP, 1.0 - oldLowerCP);
		}

		setModulationAmount(id, newAmount, true);

		double newLowerBound = oldValue - (factor * newAmount) * mcValue;
		double lowerBoundDiff = newLowerBound - oldLowerBound;
		setParameterValue(id, oldValue - lowerBoundDiff, true);
	}

	private void setModulationLowerBound(ParameterId id, double newAmount, boolean fine) {

		ModulateableParameterModel p = (ModulateableParameterModel) EditBufferModel.get().getParameter(id);
		ParameterId mcId = new ParameterId(p.modSource.getValue());
		MacroControlParameterModel mc = (MacroControlParameterModel) EditBufferModel.get().getParameter(mcId);

		double factor = p.value.metaData.bipolar.getBool() ? 2 : 1;
		double oldAmount = p.modAmount.getClippedValue();
		double oldValue = p.value.getClippedValue();
		double mcValue = mc.value.getClippedValue();
		double oldUpperBound = oldValue + (factor * oldAmount) * (1.0 - mcValue);

		if (p.value.metaData.bipolar.getBool()) {
			double oldUpperCP = NLMath.clamp((oldUpperBound + 1) / 2, 0, 1.0);
			newAmount = NLMath.clamp(newAmount, oldUpperCP - 1, oldUpperCP);
		} else {
			double oldUpperCP = NLMath.clamp(oldUpperBound, 0, 1.0);
			newAmount = NLMath.clamp(newAmount, oldUpperCP - 1, oldUpperCP);
		}

		setModulationAmount(id, newAmount, true);

		double newUpperBound = oldValue + (factor * newAmount) * (1.0 - mcValue);
		double upperBoundDiff = newUpperBound - oldUpperBound;
		setParameterValue(id, oldValue - upperBoundDiff, true);
	}

	public void setModulationSource(ParameterId id, ModSource src) {
		ModulateableParameterModel p = (ModulateableParameterModel) EditBufferModel.get().getParameter(id);
		if (p.modSource.setValue(src))
			NonMaps.get().getServerProxy().setModulationSource(src);
	}

	public IncrementalChanger startEditParameterValue(ParameterId id, double pixelsPerRange) {
		BasicParameterModel p = EditBufferModel.get().getParameter(id);
		return new IncrementalChanger(p.value, pixelsPerRange, (v, b) -> setParameterValue(id, v, true, b), () -> {
			if (p instanceof PhysicalControlParameterModel) {
				PhysicalControlParameterModel m = (PhysicalControlParameterModel) p;
				if (m.isReturning())
					startReturningAnimation(m);
			}
		});
	}

	public IncrementalChanger startEditParameterValue(int paramNumber, double pixelsPerRange) {
		ParameterId id = toParamId(paramNumber);
		return startEditParameterValue(id, pixelsPerRange);
	}

	public IncrementalChanger startEditMCAmount(ParameterId id, double pixelsPerRange) {
		ModulateableParameterModel p = (ModulateableParameterModel) EditBufferModel.get().getParameter(id);
		return new IncrementalChanger(p.modAmount, pixelsPerRange, (v, b) -> setModulationAmount(id, v, true), null);
	}

	public IncrementalChanger startEditMacroControlValue(ParameterId id, double pixelsPerRange) {
		ModulateableParameterModel p = (ModulateableParameterModel) EditBufferModel.get().getParameter(id);

		if (p.modSource.getValue() != ModSource.None)
			return startEditParameterValue(p.modSource.getValue().toParameterId(), pixelsPerRange);

		return null;
	}

	public IncrementalChanger startEditModulationAmountLowerBound(ParameterId id, double pixelsPerRange) {
		ModulateableParameterModel p = (ModulateableParameterModel) EditBufferModel.get().getParameter(id);
		return new IncrementalChanger(p.modAmount, pixelsPerRange, (v, b) -> setModulationLowerBound(id, v, true),
				null) {
			@Override
			public double bendAmount(double i) {
				return -i / 2;
			}
		};
	}

	public IncrementalChanger startEditModulationAmountUpperBound(ParameterId id, double pixelsPerRange) {
		ModulateableParameterModel p = (ModulateableParameterModel) EditBufferModel.get().getParameter(id);
		return new IncrementalChanger(p.modAmount, pixelsPerRange, (v, b) -> setModulationUpperBound(id, v, true),
				null) {
			@Override
			public double bendAmount(double i) {
				return i / 2;
			}
		};
	}

	public void renameMacroControl(ParameterId id, String newName) {
		MacroControlParameterModel m = this.<MacroControlParameterModel>findParameter(id);
		m.givenName.setValue(newName);
		NonMaps.theMaps.getServerProxy().renameMacroControl(id, newName);
	}

	public void setMacroControlInfo(ParameterId id, String text) {
		MacroControlParameterModel m = this.<MacroControlParameterModel>findParameter(id);
		m.info.setValue(text);
		NonMaps.theMaps.getServerProxy().setMacroControlInfo(id, text);
	}

	public void selectVoiceGroup(VoiceGroup group) {
		EditBufferModel.get().voiceGroup.setValue(group);
	}

	public void toggleVoiceGroup() {
		if (EditBufferModel.get().voiceGroup.getValue() == VoiceGroup.I)
			selectVoiceGroup(VoiceGroup.II);
		else if (EditBufferModel.get().voiceGroup.getValue() == VoiceGroup.II)
			selectVoiceGroup(VoiceGroup.I);
	}

	private VoiceGroup getVoiceGroupFor(int paramNumber) {
		return ParameterFactory.isGlobalParameter(paramNumber) ? VoiceGroup.Global
				: EditBufferModel.get().voiceGroup.getValue();
	}

	public void resetModulation(int id) {
		NonMaps.theMaps.getServerProxy().resetModulation(id, getVoiceGroupFor(id));
	}

	public void toggleDirectLoad() {
		LoadMode l = SetupModel.get().systemSettings.loadMode.getValue();
		SoundType s = EditBufferModel.get().soundType.getValue();

		if (s == SoundType.Single) {
			if (l != LoadMode.directload) {
				l = LoadMode.directload;
			} else {
				l = LoadMode.select;
			}
		} else {
			if (l == LoadMode.directload) {
				l = LoadMode.loadtopart;
			} else if (l == LoadMode.loadtopart) {
				l = LoadMode.select;
			} else {
				l = LoadMode.select;
			}
		}

		setLoadMode(l);
	}

	public void setLoadMode(LoadMode l) {
		SetupModel.get().systemSettings.loadMode.setValue(l);
		NonMaps.theMaps.getServerProxy().setSetting("LoadMode", l.toString());
	}

	public void loadPreset(String uuid) {
		NonMaps.theMaps.getServerProxy().loadPreset(uuid);
	}

	public void loadPresetPart(VoiceGroup i) {
		NonMaps.theMaps.getServerProxy().loadPresetPartIntoPart(i, EditBufferModel.get().voiceGroup.getValue());
	}

	public void loadSinglePresetIntoPart(String uuid, VoiceGroup loadTo) {
		NonMaps.theMaps.getServerProxy().loadPresetIntoPart(uuid, loadTo);
	}

	public void initializePart() {
		NonMaps.get().getServerProxy().initPart(EditBufferModel.get().voiceGroup.getValue());
	}

	public void renamePart(String newName) {
		NonMaps.get().getServerProxy().renamePart(EditBufferModel.get().voiceGroup.getValue(), newName);
	}

	public void randomize() {
		if (EditBufferModel.get().soundType.getValue() == SoundType.Single)
			NonMaps.get().getServerProxy().randomizeSound();
		else
			NonMaps.get().getServerProxy().randomizePart(EditBufferModel.get().voiceGroup.getValue());
	}

}
