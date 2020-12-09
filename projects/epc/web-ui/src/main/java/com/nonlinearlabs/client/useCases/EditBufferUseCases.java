package com.nonlinearlabs.client.useCases;

import com.nonlinearlabs.client.Millimeter;
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

		if (p.id.getNumber() == 356) {
			handleSplitExceptionalParameterChange(p, id, newValue, oracle);
			return;
		}

		NonMaps.get().getServerProxy().setParameter(id, newValue, oracle);
	}

	private BasicParameterModel getSibling(BasicParameterModel p) {
		if (p.id.getVoiceGroup() == VoiceGroup.Global)
			return null;

		VoiceGroup inverted = p.id.getVoiceGroup() == VoiceGroup.I ? VoiceGroup.II : VoiceGroup.I;
		return EditBufferModel.get().getParameter(new ParameterId(p.id.getNumber(), inverted));
	}

	private double splitDelta() {
		return 0.016666667;
	}

	private double splitDeltaSigned(BasicParameterModel split) {
		if (split.id.getVoiceGroup() == VoiceGroup.I) {
			return splitDelta();
		} else {
			return -splitDelta();
		}
	}

	private double getSplitEdge(BasicParameterModel split) {
		if (split.id.getVoiceGroup() == VoiceGroup.I) {
			return 1 - splitDelta();
		} else {
			return splitDelta();
		}
	}

	private double getSplitMaxOfOther(BasicParameterModel split) {
		if (split.id.getVoiceGroup() == VoiceGroup.I) {
			return 1.0;
		} else {
			return 0.0;
		}
	}

	private boolean inEdge(BasicParameterModel p, double newVal) {
		if (p.id.getVoiceGroup() == VoiceGroup.II && newVal <= getSplitEdge(p)) {
			return true;
		} else if (p.id.getVoiceGroup() == VoiceGroup.I && newVal >= getSplitEdge(p)) {
			return true;
		}
		return false;
	}

	private void handleSplitSync(BasicParameterModel p, ParameterId id, double newValue) {
		BasicParameterModel other = getSibling(p);

		if (inEdge(p, newValue)) {
			p.value.value.setValue(getSplitEdge(p));
			other.value.value.setValue(getSplitMaxOfOther(p));
			NonMaps.get().getServerProxy().setSplitPoints(p.id, p.value.value.getValue(), other.value.value.getValue(),
					true);
		} else {
			other.value.value.setValue(p.value.value.getValue() + splitDeltaSigned(p));
			NonMaps.get().getServerProxy().setSplitPoints(p.id, p.value.value.getValue(), other.value.value.getValue(),
					true);
		}
	}

	private boolean splitsHaveNegativeOverlap(double newValue, BasicParameterModel other) {
		boolean isNewValueOfI = other.id.getVoiceGroup() == VoiceGroup.II;

		int compI = (int) (newValue * 1000);
		int compII = (int) (other.value.value.getValue() * 1000);

		if (isNewValueOfI) {
			return compI < compII;
		} else {
			return compI > compII;
		}
	}

	private void preventNegativeOverlap(BasicParameterModel p, ParameterId id, double newValue, boolean oracle) {
		BasicParameterModel other = getSibling(p);

		if (splitsHaveNegativeOverlap(newValue, other)) {
			other.value.value.setValue(newValue + splitDeltaSigned(p));
		}

		NonMaps.get().getServerProxy().setSplitPoints(p.id, newValue, other.value.value.getValue(), oracle);
	}

	private void handleSplitExceptionalParameterChange(BasicParameterModel p, ParameterId id, double newValue,
			boolean oracle) {
		if (SetupModel.get().systemSettings.syncSplit.getBool()) {
			handleSplitSync(p, id, newValue);
		} else {
			preventNegativeOverlap(p, id, newValue, oracle);
		}
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

	public void incDecParameter(int paramNumber, boolean fine, int inc) {
		ParameterId id = toParamId(paramNumber);
		incDecParameter(id, fine, inc);
	}

	public void incDecParameter(ParameterId id, boolean fine, int inc) {
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

	public void setToDefault(ParameterId id) {
		NonMaps.get().getServerProxy().defaultParameter(id);
	}

	public void toggleBoolean(int paramNumber) {
		ParameterId id = toParamId(paramNumber);
		BasicParameterModel p = EditBufferModel.get().getParameter(id);

		boolean isOracle = !(p instanceof ModulationRouterParameterModel);

		if (p.value.getQuantizedAndClipped(true) != 0.0)
			setParameterValue(id, 0, isOracle);
		else
			setParameterValue(id, 1, isOracle);
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
			NonMaps.get().getServerProxy().setModulationAmount(newValue, id);
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
		double newLowerBound = oldValue - (factor * newAmount) * mcValue;
		double lowerBoundDiff = newLowerBound - oldLowerBound;
		double newParameterValue = oldValue - lowerBoundDiff;

		setModulationBounds(id, newAmount, newParameterValue);
	}

	private void setModulationBounds(ParameterId id, double newAmount, double newParameterValue) {
		ModulateableParameterModel p = (ModulateableParameterModel) EditBufferModel.get().getParameter(id);
		p.modAmount.value.setValue(newAmount);
		p.value.value.setValue(newParameterValue);
		NonMaps.get().getServerProxy().setModulationBounds(id, newAmount, newParameterValue);
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
		double newUpperBound = oldValue + (factor * newAmount) * (1.0 - mcValue);
		double upperBoundDiff = newUpperBound - oldUpperBound;
		double newParameterValue = oldValue - upperBoundDiff;

		setModulationBounds(id, newAmount, newParameterValue);
	}

	public void setModulationSource(ParameterId id, ModSource src) {
		ModulateableParameterModel p = (ModulateableParameterModel) EditBufferModel.get().getParameter(id);
		if (p.modSource.setValue(src))
			NonMaps.get().getServerProxy().setModulationSource(src, id);
	}

	public IncrementalChanger startEditParameterValue(ParameterId id, double pixelsPerRange) {
		BasicParameterModel p = EditBufferModel.get().getParameter(id);
		if (p.value.getValue().metaData.isBoolean.getBool()) {
			pixelsPerRange = Millimeter.toPixels(10);
		}

		return new IncrementalChanger(p.value, pixelsPerRange, (v, b) -> {
			setParameterValue(id, v, true, b);
		}, () -> {
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
		if(SetupModel.get().systemSettings.syncVoiceGroups.getBool()) {
			NonMaps.theMaps.getServerProxy().syncVoiceGroup();
		}
	}

	public void toggleVoiceGroup() {
		if (EditBufferModel.get().voiceGroup.getValue() == VoiceGroup.I)
			selectVoiceGroup(VoiceGroup.II);
		else if (EditBufferModel.get().voiceGroup.getValue() == VoiceGroup.II)
			selectVoiceGroup(VoiceGroup.I);
	}

	private boolean containsElement(int e, int[] arr) {
		for (int i : arr) {
			if (e == i)
				return true;
		}
		return false;
	}

	private VoiceGroup getVoiceGroupFor(int paramNumber) {
		if (EditBufferModel.get().soundType.getValue() == SoundType.Layer) {
			if (containsElement(paramNumber, ParameterFactory.voicesParameters)) {
				return VoiceGroup.I;
			}
		}

		return ParameterFactory.isGlobalParameter(paramNumber) ? VoiceGroup.Global
				: EditBufferModel.get().voiceGroup.getValue();
	}

	public void resetModulation(int id) {
		NonMaps.theMaps.getServerProxy().resetModulation(id, getVoiceGroupFor(id));
	}

	public void toggleDirectLoad() {
		if (SetupModel.get().systemSettings.directLoad.getBool()) {
			SetupModel.get().systemSettings.directLoad.setValue(false);
			NonMaps.theMaps.getServerProxy().setSetting("DirectLoad", "off");
		} else {
			SetupModel.get().systemSettings.directLoad.setValue(true);
			NonMaps.theMaps.getServerProxy().setSetting("DirectLoad", "on");
		}
	}

	public void toggleSyncSplit() {
		if (SetupModel.get().systemSettings.syncSplit.getBool()) {
			SetupModel.get().systemSettings.syncSplit.setValue(false);
			NonMaps.theMaps.getServerProxy().setSetting("SyncSplit", "off");
		} else {
			SetupModel.get().systemSettings.syncSplit.setValue(true);
			NonMaps.theMaps.getServerProxy().setSetting("SyncSplit", "on");
		}
	}

	public void setExternalMidiSetting(boolean b) {
		SetupModel.get().systemSettings.externalMidi.setValue(b);
		NonMaps.theMaps.getServerProxy().setSetting("ExternalMidi", b ? "on" : "off");
	}

	public void loadPreset(String uuid) {
		NonMaps.theMaps.getServerProxy().loadPreset(uuid);
	}

	public void loadPresetPart(String uuid, VoiceGroup i) {
		NonMaps.theMaps.getServerProxy().loadPresetPartIntoPart(uuid, i, EditBufferModel.get().voiceGroup.getValue());
	}

	public void loadPresetPartIntoPart(String uuid, VoiceGroup from, VoiceGroup to) {
		NonMaps.get().getServerProxy().loadPresetPartIntoPart(uuid, from, to);
	}

	public void initializePart() {
		NonMaps.get().getServerProxy().initPart(EditBufferModel.get().voiceGroup.getValue());
	}

	public void renamePart(String newName) {
		NonMaps.get().getServerProxy().renamePart(EditBufferModel.get().voiceGroup.getValue(), newName);
	}

	public void renamePart(String newName, VoiceGroup vg) {
		NonMaps.get().getServerProxy().renamePart(vg, newName);
	}

	public void randomize() {
		if (EditBufferModel.get().soundType.getValue() == SoundType.Single)
			NonMaps.get().getServerProxy().randomizeSound();
		else
			NonMaps.get().getServerProxy().randomizePart(EditBufferModel.get().voiceGroup.getValue());
	}

	public void roundParameter(int paramNumber) {
		ParameterId id = toParamId(paramNumber);
		ModulateableParameterModel p = (ModulateableParameterModel) EditBufferModel.get().getParameter(id);
		double rounded = p.value.getQuantizedAndClipped(false);
		setParameterValue(id, rounded, true);

	}

	public void unmutePart(VoiceGroup group) {
		NonMaps.get().getServerProxy().unmutePart(group);
	}

	public void mutePart(VoiceGroup group) {
		NonMaps.get().getServerProxy().mutePart(group);
	}

	public void setMuteForPartAndUnmuteOther(VoiceGroup g) {
		NonMaps.get().getServerProxy().setMuteForPartAndUnmuteOther(g);
	}

}
