package com.nonlinearlabs.NonMaps.client.world.overlay.belt.parameters;

import com.google.gwt.canvas.dom.client.Context2d;
import com.google.gwt.event.dom.client.KeyCodes;
import com.google.gwt.event.dom.client.KeyDownEvent;
import com.nonlinearlabs.NonMaps.client.Millimeter;
import com.nonlinearlabs.NonMaps.client.Tracer;
import com.nonlinearlabs.NonMaps.client.tools.NLMath;
import com.nonlinearlabs.NonMaps.client.world.Control;
import com.nonlinearlabs.NonMaps.client.world.Position;
import com.nonlinearlabs.NonMaps.client.world.Range;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.ModulatableParameter;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.Parameter;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.Parameter.Initiator;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.PhysicalControlParameter;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.SelectionListener;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.PlayControls.MacroControls.MacroControlParameter;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.PlayControls.MacroControls.Macros.MacroControls;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.value.ModulationAmount;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.value.QuantizedClippedValue;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.value.QuantizedClippedValue.IncrementalChanger;
import com.nonlinearlabs.NonMaps.client.world.overlay.OverlayControl;
import com.nonlinearlabs.NonMaps.client.world.overlay.OverlayLayout;
import com.nonlinearlabs.NonMaps.client.world.overlay.belt.Belt;
import com.nonlinearlabs.NonMaps.client.world.overlay.layouter.HarmonicLayouter;
import com.nonlinearlabs.NonMaps.client.world.pointer.TouchPinch;

public class BeltParameterLayout extends OverlayLayout implements SelectionListener {

	enum Mode {
		unmodulateableParameter, modulateableParameter, paramValue, mcValue, mcAmount, mcLower, mcUpper, mcSource
	}

	private Mode mode = Mode.modulateableParameter;
	private IncrementalChanger currentIncrementalChanger;

	private OverlayControl modulationButtons;
	private OverlayControl slider;
	private OverlayControl mcSourceDisplay;
	private OverlayControl editorMode;
	private OverlayControl valueDisplay;
	private OverlayControl parameterName;
	private OverlayControl dottedLine;
	private OverlayControl infoButton;
	private OverlayControl contextMenu;

	private MCRadioButton mcPositionRadioButton;
	private MCRadioButton mcAmountRadioButton;
	private MCRadioButton mcLowerBoundRadioButton;
	private MCRadioButton mcUpperBoundRadioButton;
	private ParameterClippingLabel mcUpperClip;
	private ParameterClippingLabel mcLowerClip;

	private QuantizedClippedValue currentValue;

	public BeltParameterLayout(Belt parent) {
		super(parent);

		addChild(modulationButtons = new ModulationButtons(this));
		addChild(mcSourceDisplay = new MCSourceDisplay(this));
		addChild(slider = new Sliders(this));
		addChild(editorMode = new EditorModeButton(this));
		addChild(valueDisplay = new ValueDisplay(this));
		addChild(parameterName = new CurrentValuesName(this));
		addChild(dottedLine = new DottedLine(this));
		addChild(infoButton = new InfoButton(this));

		addChild(contextMenu = new ContextMenuButton(this));

		addChild(mcPositionRadioButton = new MCRadioButton(this, Mode.mcValue, "MC_Pos_Aktiv.svg",
				"MC_Pos_Inaktiv.svg"));
		addChild(
				mcAmountRadioButton = new MCRadioButton(this, Mode.mcAmount, "MC_Amt_Aktiv.svg", "MC_Amt_Inaktiv.svg"));
		addChild(mcLowerBoundRadioButton = new MCRadioButton(this, Mode.mcLower, "MC_Low_Aktiv.svg",
				"MC_Low_Inaktiv.svg"));
		addChild(mcUpperBoundRadioButton = new MCRadioButton(this, Mode.mcUpper, "MC_High_Aktiv.svg",
				"MC_High_Inaktiv.svg"));

		addChild(mcUpperClip = new ParameterClippingLabel(this, Mode.mcUpper));
		addChild(mcLowerClip = new ParameterClippingLabel(this, Mode.mcLower));

		getNonMaps().getNonLinearWorld().getParameterEditor().registerListener(this);

		setupValue();
	}

	public Mode getMode() {
		return mode;
	}

	@Override
	public Belt getParent() {
		return (Belt) super.getParent();
	}

	@Override
	public void draw(Context2d ctx, int invalidationMask) {
		fixMode();
		super.draw(ctx, invalidationMask);
	}

	private void fixMode() {
		if (!isModulationAssigned()) {
			if (isOneOf(Mode.mcValue, Mode.mcAmount, Mode.mcLower, Mode.mcUpper))
				setMode(Mode.mcSource);

			if (mcPositionRadioButton.isIntendedVisible())
				showAndHideChildren();
		} else {
			if (!mcPositionRadioButton.isIntendedVisible())
				showAndHideChildren();
		}
	}

	@Override
	public void doLayout(double x, double y, double w, double h) {
		super.doLayout(x, y, w, h);

		double undoWidth = Millimeter.toPixels(30);
		double undoRedoMargin = Millimeter.toPixels(4.5);

		double third = h / 3.0;
		final double buttonDim = Millimeter.toPixels(10);

		double editorModeLeft = (Millimeter.toPixels(70) / 4) + (buttonDim / 2);
		double modSrcDim = Millimeter.toPixels(10);
		double modulationButtonsDim = Millimeter.toPixels(25);
		double modulationButtonsLeft = Millimeter.toPixels(19.5 - 22.5 / 2);
		double sliderLeft = modulationButtonsLeft + modulationButtonsDim + Millimeter.toPixels(10);

		modulationButtons.doLayout(modulationButtonsLeft, (h - modulationButtonsDim) / 2, modulationButtonsDim,
				modulationButtonsDim);
		mcSourceDisplay.doLayout(undoRedoMargin + undoWidth * 0.75 - modSrcDim / 2, (h - modSrcDim) / 2, modSrcDim,
				modSrcDim);
		editorMode.doLayout(w - editorModeLeft, (h - buttonDim) / 2, buttonDim, buttonDim);

		double clipW = 20;
		mcLowerClip.doLayout(sliderLeft - clipW, third, clipW, third);
		slider.doLayout(sliderLeft, third, w - sliderLeft - sliderLeft, third);
		mcUpperClip.doLayout(sliderLeft + w - sliderLeft - sliderLeft, third, clipW, third);

		double upperElementsY = Millimeter.toPixels(0.5);

		double sliderWidth = slider.getRelativePosition().getWidth();
		double margin = Millimeter.toPixels(2.5);
		double modulationButtonWidth = 1.5 * mcPositionRadioButton.getSelectedImage().getImgWidth();

		HarmonicLayouter layouter = new HarmonicLayouter();

		layouter.push(null, margin, margin, 0, 0);
		layouter.push(null, 0, sliderWidth, 0, 2);
		layouter.push(mcPositionRadioButton, modulationButtonWidth, modulationButtonWidth, 1, 2);
		layouter.push(null, margin, margin, 0, 2);
		layouter.push(mcAmountRadioButton, modulationButtonWidth, modulationButtonWidth, 1, 2);
		layouter.push(null, margin, margin, 0, 2);

		layouter.push(valueDisplay, modulationButtonWidth, sliderWidth * 0.75, 2, 1);

		layouter.push(null, margin, margin, 0, 2);
		layouter.push(mcLowerBoundRadioButton, modulationButtonWidth, modulationButtonWidth, 1, 2);
		layouter.push(null, margin, margin, 0, 2);
		layouter.push(mcUpperBoundRadioButton, modulationButtonWidth, modulationButtonWidth, 1, 2);
		layouter.push(null, 0, sliderWidth, 0, 2);
		layouter.push(null, margin, margin, 0, 0);

		double walkerX = sliderLeft;

		double modAndParamValueYValue = h / 2.3;

		for (HarmonicLayouter.Result r : layouter.layout(sliderWidth)) {
			if (r.record.attached != null) {
				OverlayControl c = (OverlayControl) r.record.attached;
				c.doLayout(walkerX, 0, r.width, modAndParamValueYValue);
			}
			walkerX += r.width;
		}

		parameterName.doLayout(sliderLeft, 2 * third - upperElementsY, slider.getRelativePosition().getWidth(), third);

		double dottedLineInset = 5;
		double lineWidth = slider.getRelativePosition().getLeft() - mcSourceDisplay.getRelativePosition().getRight()
				+ dottedLineInset;

		dottedLine.doLayout(mcSourceDisplay.getRelativePosition().getRight() - dottedLineInset, 0, lineWidth, h);

		infoButton.doLayout(undoRedoMargin + undoWidth / 4 - modSrcDim / 2, (h - modSrcDim) / 2, modSrcDim, modSrcDim);
		contextMenu.doLayout(undoRedoMargin + undoWidth * 0.75 - modSrcDim / 2, (h - modSrcDim) / 2, modSrcDim,
				modSrcDim);
	}

	protected void showAndHideChildren() {
		modulationButtons.setVisible(
				isOneOf(Mode.mcValue, Mode.mcAmount, Mode.mcSource, Mode.mcLower, Mode.mcUpper, Mode.paramValue));
		mcSourceDisplay.setVisible(isOneOf(Mode.modulateableParameter));
		editorMode.setVisible(true);
		slider.setVisible(true);
		parameterName.setVisible(true);

		boolean modAssigned = isModulationAssigned();

		mcPositionRadioButton.setVisible(modAssigned
				&& isOneOf(Mode.mcValue, Mode.mcAmount, Mode.mcSource, Mode.mcLower, Mode.mcUpper, Mode.paramValue));
		mcAmountRadioButton.setVisible(modAssigned
				&& isOneOf(Mode.mcValue, Mode.mcAmount, Mode.mcSource, Mode.mcLower, Mode.mcUpper, Mode.paramValue));
		mcLowerBoundRadioButton.setVisible(modAssigned
				&& isOneOf(Mode.mcValue, Mode.mcAmount, Mode.mcSource, Mode.mcLower, Mode.mcUpper, Mode.paramValue));
		mcUpperBoundRadioButton.setVisible(modAssigned
				&& isOneOf(Mode.mcValue, Mode.mcAmount, Mode.mcSource, Mode.mcLower, Mode.mcUpper, Mode.paramValue));

		valueDisplay.setVisible(isOneOf(Mode.mcValue, Mode.mcAmount, Mode.mcSource, Mode.mcLower, Mode.mcUpper,
				Mode.paramValue, Mode.modulateableParameter, Mode.unmodulateableParameter));

		dottedLine.setVisible(isOneOf(Mode.modulateableParameter));
		infoButton.setVisible(isOneOf(Mode.modulateableParameter, Mode.unmodulateableParameter));

		Parameter p = getNonMaps().getNonLinearWorld().getParameterEditor().getSelectedOrSome();
		boolean ctxMenuVisible = isOneOf(Mode.unmodulateableParameter) && p.hasContextMenu()
				&& existsMoreThanOneItemInContextMenu(p);
		contextMenu.setVisible(ctxMenuVisible);
	}

	private boolean existsMoreThanOneItemInContextMenu(Parameter p) {
		return (p instanceof ModulatableParameter || p instanceof PhysicalControlParameter
				|| p instanceof MacroControlParameter) && !p.getName().getLongName().equals("Bender")
				&& !p.getName().getLongName().equals("Aftertouch");
	}

	protected void toggleMcEditMode(Mode m) {
		if (mode != m)
			setMode(m);
		else
			setMode(Mode.paramValue);
	}

	public void setMode(Mode m) {
		setMode(m, false);
	}

	public void setMode(Mode m, boolean force) {
		if (mode != m || force) {
			mode = m;
			setupValue();
			showAndHideChildren();
			requestLayout();
		}
	}

	public boolean isModulationAssigned() {
		Parameter p = getNonMaps().getNonLinearWorld().getParameterEditor().getSelectedOrSome();
		if (p instanceof ModulatableParameter) {
			ModulatableParameter m = (ModulatableParameter) p;
			return m.getModulationSource() != MacroControls.NONE;
		}
		return false;
	}

	private void setupValue() {
		Parameter p = getNonMaps().getNonLinearWorld().getParameterEditor().getSelectedOrSome();

		if (p instanceof ModulatableParameter) {
			ModulatableParameter m = (ModulatableParameter) p;

			if (m.getModulationSource() != MacroControls.NONE) {
				switch (getMode()) {
				case mcAmount:
					currentValue = m.getModulationAmount();
					return;

				case mcValue:
					MacroControls s = m.getModulationSource();
					Parameter q = getNonMaps().getNonLinearWorld().getParameterEditor().getMacroControls()
							.getControl(s);
					currentValue = q.getValue();
					return;

				case mcLower:
					currentValue = createLowerBoundDummyValue(m);
					return;

				case mcUpper:
					currentValue = createUpperBoundDummyValue(m);
					return;

				default:
				}
			}
		}
		currentValue = p.getValue();
	}

	public boolean isOneOf(Mode... m) {
		return isModeOneOf(mode, m);
	}

	public boolean isModeOneOf(Mode inQuestion, Mode... m) {
		for (Mode u : m)
			if (inQuestion == u)
				return true;

		return false;
	}

	@Override
	public void onSelectionChanged(Parameter oldSelection, final Parameter newSelection) {
		boolean newModulateable = newSelection instanceof ModulatableParameter;

		currentIncrementalChanger = null;

		if (newModulateable)
			setMode(Mode.modulateableParameter, true);
		else
			setMode(Mode.unmodulateableParameter, true);
	}

	public void setValueChanger(IncrementalChanger changer) {
		currentIncrementalChanger = changer;
	}

	@Override
	public Control mouseDrag(Position oldPoint, Position newPoint, boolean fine) {
		if (currentIncrementalChanger != null) {
			double amount = newPoint.getX() - oldPoint.getX();
			currentIncrementalChanger.changeBy(fine, amount);
		}
		return this;
	}

	@Override
	public Control pinch(Position eventPoint, double touchDist, TouchPinch pinch) {
		if (currentIncrementalChanger != null) {
			double maxDiff = pinch.getMaxTouchDistance();
			currentIncrementalChanger.changeBy(true, maxDiff);
		} else {
			mouseDown(eventPoint);
		}

		return this;
	}

	@Override
	public Control mouseDown(Position eventPoint) {
		currentIncrementalChanger = currentValue.startUserEdit(slider.getPixRect().getWidth());
		return this;
	}

	private QuantizedClippedValue createBoundDummyValue(ModulatableParameter p, final boolean upper) {
		QuantizedClippedValue dummyValue = new QuantizedClippedValue(new UpperLowerBoundListener(upper));
		dummyValue.setCoarseDenominator(p.getValue().getCoarseDenominator());
		dummyValue.setFineDenominator(p.getValue().getFineDenominator());
		dummyValue.setBipolar(p.getValue().isBipolar());
		dummyValue.setRawValue(Initiator.INDIRECT_USER_ACTION, calcBound(upper));
		return dummyValue;
	}

	public double calcBound(boolean upper) {
		Parameter p = getNonMaps().getNonLinearWorld().getParameterEditor().getSelectedOrSome();

		if (p instanceof ModulatableParameter) {
			ModulatableParameter m = (ModulatableParameter) p;
			MacroControls s = m.getModulationSource();
			if (s != MacroControls.NONE) {

				ModulationAmount amount = m.getModulationAmount();
				MacroControlParameter mc = getNonMaps().getNonLinearWorld().getParameterEditor().getMacroControls()
						.getControl(s);
				double modAmount = amount.getClippedValue();

				if (m.isBiPolar())
					modAmount *= 2;

				double srcValue = mc.getValue().getClippedValue();
				double value = m.getValue().getClippedValue();
				double modLeft = value - modAmount * srcValue;

				if (!upper)
					return m.getValue().clip(modLeft);
				else
					return m.getValue().clip(modLeft + modAmount);
			}
		}
		return 0;
	}

	private QuantizedClippedValue createLowerBoundDummyValue(ModulatableParameter m) {
		return createBoundDummyValue(m, false);
	}

	private QuantizedClippedValue createUpperBoundDummyValue(ModulatableParameter m) {
		return createBoundDummyValue(m, true);
	}

	@Override
	public void onMouseLost() {
		if (currentIncrementalChanger != null) {
			currentIncrementalChanger.finish();
		}
		super.onMouseLost();
	}

	@Override
	public Control doubleClick() {
		if (currentIncrementalChanger != null && mode == Mode.paramValue) {
			currentIncrementalChanger.getValue().setToDefault(Parameter.Initiator.EXPLICIT_USER_ACTION);
			Tracer.log("Set to Default! " + mode.toString());
		}
		return this;
	}

	@Override
	public Control onKey(final KeyDownEvent event) {
		if (currentIncrementalChanger == null) {
			if (currentValue == null)
				setupValue();

			if (currentValue != null)
				currentIncrementalChanger = currentValue.startUserEdit(slider.getPixRect().getWidth());
		}

		if (currentIncrementalChanger != null) {
			if (event.getNativeKeyCode() == KeyCodes.KEY_K) {
				currentIncrementalChanger.getValue().inc(Parameter.Initiator.EXPLICIT_USER_ACTION,
						event.isShiftKeyDown());
				return this;
			} else if (event.getNativeKeyCode() == KeyCodes.KEY_M) {
				currentIncrementalChanger.getValue().dec(Parameter.Initiator.EXPLICIT_USER_ACTION,
						event.isShiftKeyDown());
				return this;
			}
		}

		return null;
	}

	@Override
	public Control wheel(Position eventPoint, double amount, boolean fine) {
		if (amount > 0)
			getValue().inc(Initiator.EXPLICIT_USER_ACTION, fine);
		else if (amount < 0)
			getValue().dec(Initiator.EXPLICIT_USER_ACTION, fine);

		return this;
	}

	public QuantizedClippedValue getValue() {
		return currentValue;
	}

	public String getCurrentValuesName(double width) {
		Parameter p = getNonMaps().getNonLinearWorld().getParameterEditor().getSelectedOrSome();
		return p.getFullNameWithGroup();
	}

	public String[] getDecoratedValue(boolean b) {
		Parameter p = getNonMaps().getNonLinearWorld().getParameterEditor().getSelectedOrSome();

		if (p instanceof ModulatableParameter) {
			ModulatableParameter m = (ModulatableParameter) p;
			MacroControls s = m.getModulationSource();

			if (s != MacroControls.NONE) {
				ModulationAmount amount = m.getModulationAmount();
				MacroControlParameter mc = getNonMaps().getNonLinearWorld().getParameterEditor().getMacroControls()
						.getControl(s);
				double modAmount = amount.getClippedValue();
				Tracer.log("modAmount before Quantize: " + modAmount);
				modAmount = NLMath.quantize(modAmount, 1000);

				if (m.isBiPolar())
					modAmount *= 2;

				double srcValue = mc.getValue().getClippedValue();
				double value = m.getValue().getClippedValue();

				Range bounds = new Range(m.isBiPolar() ? -1.0 : 0, 1.0);
				Range mod = new Range(value - modAmount * srcValue, value - modAmount * srcValue + modAmount);
				Range modNormalized = new Range(mod.getLeft(), mod.getRight());
				modNormalized.normalize();

				double r = NLMath.quantize(modNormalized.getRight(), 1000);
				double l = NLMath.quantize(modNormalized.getLeft(), 1000);

				Tracer.log("modAmount: " + modAmount);
				Tracer.log("mcAmt: " + m.getModulationAmount().getDecoratedValue(false));
				Tracer.log("mcLower: " + p.getDecoratedValue(false, mod.getLeft()));
				Tracer.log("mcUpper: " + p.getDecoratedValue(false, mod.getRight()));
				Tracer.log("mcValue: " + mc.getDecoratedValue(false));
				Tracer.log(bounds.getLeft() + " - " + bounds.getRight());

				
				mcUpperClip.setClipping(bounds.outOfRange(r));
				mcLowerClip.setClipping(bounds.outOfRange(l));

				switch (mode) {
				case mcAmount: {
					String with = m.getModulationAmount().getDecoratedValue(true);
					String without = m.getModulationAmount().getDecoratedValue(false);
					return new String[] { "MC Amount: " + with, "MC Amount: " + without, "MC Amt: " + without,
							"Amt: " + without, without };
				}

				case mcLower: {
					String clip = bounds.outOfRange(modAmount >= 0 ? l : r) ? "! " : "";
					mod.clipTo(bounds);
					String with = p.getDecoratedValue(true, mod.getLeft());
					String without = p.getDecoratedValue(false, mod.getLeft());
					return new String[] { clip + "Lower Limit: " + with, clip + "Lower Limit: " + without,
							clip + "Lower: " + without, clip + "Lo: " + without, clip + without };
				}

				case mcUpper: {
					String clip = bounds.outOfRange(modAmount >= 0 ? r : l) ? "! " : "";
					mod.clipTo(bounds);
					String with = p.getDecoratedValue(true, mod.getRight());
					String without = p.getDecoratedValue(false, mod.getRight());
					return new String[] { clip + "Upper Limit: " + with, clip + "Upper Limit: " + without,
							clip + "Upper: " + without, clip + "Up: " + without, clip + without };
				}

				case mcValue: {
					String with = mc.getDecoratedValue(true);
					String without = mc.getDecoratedValue(false);
					return new String[] { "MC Position: " + with, "MC Position: " + without, "MC Pos: " + without,
							"Pos: " + without, without };
				}

				case mcSource:
				case modulateableParameter:
				case paramValue:
				case unmodulateableParameter:
					break;
				}
			}
		}

		return new String[] { p.getDecoratedValue(true), p.getDecoratedValue(false) };
	}

}
