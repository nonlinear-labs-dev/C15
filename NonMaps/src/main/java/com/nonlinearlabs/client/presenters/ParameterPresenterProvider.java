package com.nonlinearlabs.client.presenters;

import java.util.Arrays;
import java.util.Set;
import java.util.TreeSet;

import com.nonlinearlabs.client.dataModel.Notifier;
import com.nonlinearlabs.client.dataModel.editBuffer.AftertouchParameterModel;
import com.nonlinearlabs.client.dataModel.editBuffer.BasicParameterModel;
import com.nonlinearlabs.client.dataModel.editBuffer.BenderParameterModel;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel;
import com.nonlinearlabs.client.dataModel.editBuffer.MacroControlParameterModel;
import com.nonlinearlabs.client.dataModel.editBuffer.ModulateableParameterModel;
import com.nonlinearlabs.client.dataModel.editBuffer.ModulateableParameterModel.ModSource;
import com.nonlinearlabs.client.dataModel.editBuffer.ModulationRouterParameterModel;
import com.nonlinearlabs.client.dataModel.editBuffer.ParameterId;
import com.nonlinearlabs.client.dataModel.editBuffer.PedalParameterModel;
import com.nonlinearlabs.client.dataModel.editBuffer.PedalParameterModel.Modes;
import com.nonlinearlabs.client.dataModel.editBuffer.PhysicalControlParameterModel;
import com.nonlinearlabs.client.dataModel.editBuffer.RibbonParameterModel;
import com.nonlinearlabs.client.dataModel.editBuffer.RibbonParameterModel.ReturnModes;
import com.nonlinearlabs.client.dataModel.setup.SetupModel;
import com.nonlinearlabs.client.dataModel.setup.SetupModel.BooleanValues;
import com.nonlinearlabs.client.tools.NLMath;
import com.nonlinearlabs.client.world.Range;

public class ParameterPresenterProvider extends Notifier<ParameterPresenter> {
	private ParameterPresenter presenter = new ParameterPresenter();

	static Set<Integer> handleOnlyParameters = new TreeSet<Integer>(
			Arrays.asList(135, 155, 254, 259, 264, 269, 284, 289, 274, 279, 243, 244, 245, 246));

	public ParameterPresenterProvider(ParameterId parameterId) {

		presenter.id = parameterId;

		BasicParameterModel p = EditBufferModel.get().getParameter(parameterId);
		p.onChange(e -> {
			updatePresenter(e);
			return true;
		});

		EditBufferModel.get().selectedParameter.onChange(id -> {
			boolean isSelected = presenter.id.getNumber() == id;
			if (isSelected != presenter.selected) {
				presenter.selected = isSelected;
				presenter.updateHash();
				notifyChanges();
			}
			return true;
		});

		if (parameterId.getNumber() == 356) {
			EditBufferModel.get().voiceGroup.onChange(v -> {
				updatePresenter(p);
				return true;
			});
		}
	}

	private void updatePresenter(BasicParameterModel e) {
		presenter.parameterInfo = e.info.getValue();
		presenter.isBoolean = e.value.metaData.isBoolean.getBool();
		presenter.drawCenterReturnIndicator = false;
		presenter.drawZeroReturnIndicator = false;
		presenter.isReturning = false;
		presenter.drawHandleOnly = handleOnlyParameters.contains(e.id.getNumber());
		presenter.highlightChanged = false;
		presenter.modulation.isModulateable = false;
		presenter.modulation.lowerClipping = false;
		presenter.modulation.upperClipping = false;
		presenter.controlPosition = e.value.getQuantizedAndClipped(true);
		presenter.bipolar = e.value.metaData.bipolar.getValue() == BooleanValues.on;

		if (e.id.getNumber() == 356 && EditBufferModel.get().voiceGroup.getValue() == EditBufferModel.VoiceGroup.II) {
			double nextValue = e.value.getIncDecValue(false, 1);
			presenter.displayValues = new String[] { e.value.getDecoratedValue(true, nextValue, true),
					e.value.getDecoratedValue(false, nextValue, true) };
		} else {
			presenter.displayValues = new String[] { e.value.getDecoratedValue(true, true),
					e.value.getDecoratedValue(false, true) };
		}
		presenter.originalParameterValueDecoratedString = e.value.getDecoratedValue(true, e.originalValue.getValue(),
				true);

		presenter.shortName = e.shortName.getValue();
		presenter.longName = e.longName.getValue();
		presenter.parameterInfo = e.info.getValue();

		presenter.valueChanged = isValueChanged(e);

		presenter.showContextMenu = false;

		if (e instanceof ModulateableParameterModel)
			updatePresenter((ModulateableParameterModel) e);

		if (e instanceof PhysicalControlParameterModel)
			updatePresenter((PhysicalControlParameterModel) e);

		if (e instanceof MacroControlParameterModel)
			updatePresenter((MacroControlParameterModel) e);

		if (e instanceof ModulationRouterParameterModel)
			updatePresenter((ModulationRouterParameterModel) e);

		presenter.changed = presenter.valueChanged || presenter.modulation.isModAmountChanged
				|| presenter.modulation.isModSourceChanged;

		boolean highlight = SetupModel.get().systemSettings.highlightChangedParameters.isTrue();
		boolean forceHighlight = SetupModel.get().systemSettings.forceHighlightChangedParameters.isTrue();
		presenter.highlightChanged = presenter.changed && (highlight || forceHighlight);

		boolean changed = presenter.changed;
		if (e.group == null)
			presenter.fullNameWithGroup = "---";
		else
			presenter.fullNameWithGroup = e.group.longName.getValue() + "   \u2013   " + e.longName.getValue()
					+ (changed ? " *" : "");

		if (presenter.updateHash())
			notifyChanges();
	}

	private boolean isValueChanged(BasicParameterModel e) {
		int denominator = e.value.metaData.fineDenominator.getValue();
		long rVal = Math.round(e.value.value.getValue() * denominator);
		long rOgVal = Math.round(e.originalValue.getValue() * denominator);
		return rVal != rOgVal;
	}

	private void updatePresenter(ModulationRouterParameterModel p) {
		PhysicalControlParameterModel m = (PhysicalControlParameterModel) EditBufferModel.get()
				.getParameter(p.getAssociatedPhysicalControlID());
		presenter.isBoolean = !m.isReturning();
	}

	private void updatePresenter(ModulateableParameterModel p) {
		presenter.modulation.isMCPosChanged = false;

		if (p.modSource.getValue() != ModSource.None) {
			BasicParameterModel mcBPM = EditBufferModel.get().getParameter(new ParameterId(p.modSource.getValue()));
			if (mcBPM != null && mcBPM instanceof MacroControlParameterModel) {
				presenter.modulation.isMCPosChanged = isValueChanged(mcBPM);
			}
		}

		int denominator = p.modAmount.metaData.fineDenominator.getValue();
		int ogRounded = (int) (p.ogModAmount.getValue() * denominator);
		int nowRounded = (int) (p.modAmount.value.getValue() * denominator);
		presenter.modulation.isModAmountChanged = ogRounded != nowRounded;
		presenter.modulation.isModSourceChanged = p.ogModSource.getValue() != p.modSource.getValue();
		presenter.modulation.ogModSource = p.ogModSource.getValue();

		presenter.showContextMenu = true;
		presenter.modulation.isModulateable = true;
		presenter.modulation.isModulated = p.modSource.getValue() != ModSource.None;

		if (presenter.modulation.isModulated) {
			presenter.modulation.modulationSource = p.modSource.getValue();
			presenter.modulation.modulationSourceLabel = p.modSource.getValue().toString();

			double modAmount = p.modAmount.getQuantizedAndClipped(true);
			double value = p.value.getQuantized(true);
			boolean isBiPolar = p.value.metaData.bipolar.getBool();
			double bipolarFactor = isBiPolar ? 2 : 1;

			double modLeft = 0;
			double modRight = 0;

			modAmount *= bipolarFactor;

			MacroControlParameterModel mc = (MacroControlParameterModel) EditBufferModel.get()
					.getParameter(new ParameterId(p.modSource.getValue()));

			modLeft = value - modAmount * mc.value.getQuantizedAndClipped(true);
			modRight = modLeft + modAmount;

			if (modRight < modLeft) {
				double tmp = modLeft;
				modLeft = modRight;
				modRight = tmp;
			}

			if (isBiPolar) {
				modLeft = NLMath.clamp(modLeft, -1.0, 1.0);
				modRight = NLMath.clamp(modRight, -1.0, 1.0);
			} else {
				modLeft = NLMath.clamp(modLeft, 0.0, 1.0);
				modRight = NLMath.clamp(modRight, 0.0, 1.0);
			}

			presenter.modulation.modulationRange.left = modLeft;
			presenter.modulation.modulationRange.right = modRight;

			presenter.modulation.modulationAmount = p.modAmount.getQuantizedAndClipped(true);

			String modAmountWithUnit = p.modAmount.getDecoratedValue(true, true);
			String modAmountWithoutUnit = p.modAmount.getDecoratedValue(false, true);
			presenter.modulation.amountDisplayValues = new String[] { "MC Amount: " + modAmountWithUnit,
					"MC Amount: " + modAmountWithoutUnit, "MC Amt: " + modAmountWithoutUnit,
					"Amt: " + modAmountWithoutUnit, modAmountWithoutUnit };

			double srcValue = mc.value.getClippedValue();

			Range bounds = new Range(p.value.metaData.bipolar.getBool() ? -1.0 : 0, 1.0);
			double left = (value - modAmount * srcValue);
			double right = left + modAmount;
			Range mod = new Range(left, right);
			Range modNormalized = new Range(mod.getLeft(), mod.getRight());
			modNormalized.normalize();

			double r = NLMath.quantize(modNormalized.getRight(), 1000);
			double l = NLMath.quantize(modNormalized.getLeft(), 1000);

			{

				String clip = bounds.outOfRange(modAmount >= 0 ? l : r) ? "! " : "";
				mod.clipTo(bounds);
				String with = p.value.getDecoratedValue(true, mod.getLeft() / bipolarFactor, true);
				String without = p.value.getDecoratedValue(false, mod.getLeft() / bipolarFactor, true);
				presenter.modulation.amountLowerBoundDisplayValues = new String[] { clip + "Lower Limit: " + with,
						clip + "Lower Limit: " + without, clip + "Lower: " + without, clip + "Lo: " + without,
						clip + without };
			}

			{
				String clip = bounds.outOfRange(modAmount >= 0 ? r : l) ? "! " : "";
				mod.clipTo(bounds);
				String with = p.value.getDecoratedValue(true, mod.getRight() / bipolarFactor, true);
				String without = p.value.getDecoratedValue(false, mod.getRight() / bipolarFactor, true);
				presenter.modulation.amountUpperBoundDisplayValues = new String[] { clip + "Upper Limit: " + with,
						clip + "Upper Limit: " + without, clip + "Upper: " + without, clip + "Up: " + without,
						clip + without };
			}

			{
				String with = mc.value.getDecoratedValue(true, true);
				String without = mc.value.getDecoratedValue(false, true);
				presenter.modulation.macroControlDisplayValues = new String[] { "MC Position: " + with,
						"MC Position: " + without, "MC Pos: " + without, "Pos: " + without, without };
			}

			{
				presenter.modulation.originalModulationAmountDecoratedString = p.modAmount.getDecoratedValue(true,
						p.ogModAmount.getValue(), true);

				MacroControlParameterModel macroControl = (MacroControlParameterModel) EditBufferModel.get()
						.getParameter(new ParameterId(p.modSource.getValue()));
				presenter.modulation.originalModulationPositionDecoratedString = macroControl.value
						.getDecoratedValue(true, macroControl.originalValue.getValue(), true);

			}

			updateClipping(p);
		} else {
			presenter.modulation.modulationSourceLabel = "";
			presenter.modulation.modulationSource = ModSource.None;
			presenter.modulation.amountDisplayValues = new String[] { "" };
			presenter.modulation.amountLowerBoundDisplayValues = new String[] { "" };
			presenter.modulation.amountUpperBoundDisplayValues = new String[] { "" };
			presenter.modulation.macroControlDisplayValues = new String[] { "" };
			presenter.modulation.originalModulationAmountDecoratedString = "---";
			presenter.modulation.originalModulationPositionDecoratedString = "---";
			presenter.modulation.modulationRange.left = presenter.modulation.modulationRange.right = presenter.controlPosition;
		}
	}

	private void updateClipping(ModulateableParameterModel m) {
		if (m.modSource.getValue() != ModSource.None) {
			MacroControlParameterModel mc = (MacroControlParameterModel) EditBufferModel.get()
					.getParameter(new ParameterId(m.modSource.getValue()));
			double modAmount = m.modAmount.getClippedValue();
			boolean bipolar = m.value.metaData.bipolar.getBool();

			if (bipolar)
				modAmount *= 2;

			double srcValue = mc.value.getClippedValue();
			double value = m.value.getClippedValue();

			Range bounds = new Range(bipolar ? -1.0 : 0, 1.0);
			double left = (value - modAmount * srcValue);
			double right = left + modAmount;
			Range mod = new Range(left, right);
			Range modNormalized = new Range(mod.getLeft(), mod.getRight());
			modNormalized.normalize();

			double r = NLMath.quantize(modNormalized.getRight(), 1000);
			double l = NLMath.quantize(modNormalized.getLeft(), 1000);

			presenter.modulation.lowerClipping = bounds.outOfRange(l);
			presenter.modulation.upperClipping = bounds.outOfRange(r);
		}
	}

	private void updatePresenter(PhysicalControlParameterModel p) {
		presenter.changed = false;
		presenter.showContextMenu = p.id.getNumber() != 274 && p.id.getNumber() != 279;

		if (p instanceof RibbonParameterModel) {
			RibbonParameterModel r = (RibbonParameterModel) p;
			presenter.drawCenterReturnIndicator = r.mode.getValue() == ReturnModes.return_to_center;
			presenter.drawZeroReturnIndicator = false;
			presenter.isReturning = r.mode.getValue() != ReturnModes.non_return;
		}

		if (p instanceof PedalParameterModel) {
			PedalParameterModel r = (PedalParameterModel) p;
			presenter.drawCenterReturnIndicator = r.mode.getValue() == Modes.returnToCenter;
			presenter.drawZeroReturnIndicator = r.mode.getValue() == Modes.returnToZero;
			presenter.isReturning = r.mode.getValue() != Modes.stay;
		}

		if (p instanceof BenderParameterModel) {
			presenter.drawCenterReturnIndicator = true;
			presenter.drawZeroReturnIndicator = false;
			presenter.isReturning = true;
		}

		if (p instanceof AftertouchParameterModel) {
			presenter.drawCenterReturnIndicator = false;
			presenter.drawZeroReturnIndicator = true;
			presenter.isReturning = true;
		}
	}

	private void updatePresenter(MacroControlParameterModel p) {
		presenter.userGivenName = p.givenName.getValue();
		presenter.showContextMenu = true;
		presenter.isMacroControl = false;
	}

	@Override
	public ParameterPresenter getValue() {
		return presenter;
	}

}
