package com.nonlinearlabs.client.presenters;

import com.nonlinearlabs.client.Checksum;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.VoiceGroup;
import com.nonlinearlabs.client.dataModel.editBuffer.ModulateableParameterModel.ModSource;
import com.nonlinearlabs.client.dataModel.editBuffer.ParameterId;
import com.nonlinearlabs.client.dataModel.setup.SetupModel;

public class ParameterPresenter {
	public static class Range {
		public double left = 0;
		public double right = 0;
	}

	public ParameterId id = new ParameterId(0, VoiceGroup.Global);
	public String[] displayValues = new String[] { "" };
	public double controlPosition = 0;
	public boolean bipolar = false;
	public boolean selected = false;
	public boolean locked = false;
	public boolean isBoolean = false;
	public boolean changed = false;
	public boolean valueChanged = false;
	public String shortName = "---";
	public String longName = "---";
	public String fullNameWithGroup = "";
	public boolean fillFromRightEnabled = false;
	public boolean disabled = false;
	public boolean hidden = false;

	public long hash = 0;

	public static class Modulation {
		public Range modulationRange = new Range();
		public String modulationSourceLabel = "";
		public ModSource modulationSource = ModSource.None;
		public boolean isModulated = false;
		public boolean isModulateable = false;
		public String[] amountDisplayValues = new String[] { "" };
		public String[] amountLowerBoundDisplayValues = new String[] { "" };
		public String[] amountUpperBoundDisplayValues = new String[] { "" };
		public String[] macroControlDisplayValues = new String[] { "" };
		public boolean lowerClipping = false;
		public boolean upperClipping = false;
		public String originalModulationAmountDecoratedString = "---";
		public String originalModulationPositionDecoratedString = "---";
		public double modulationAmount = 0;

		public boolean isMCPosChanged = false;
		public boolean isModAmountChanged = false;
		public boolean isModSourceChanged = false;
		public ModSource ogModSource = ModSource.None;
	}

	public Modulation modulation = new Modulation();
	public boolean highlightChanged = false;
	public String userGivenName = "";
	public boolean showContextMenu = false;
	public String originalParameterValueDecoratedString = "---";
	public boolean drawHandleOnly = false;
	public boolean drawCenterReturnIndicator = false;
	public boolean drawZeroReturnIndicator = false;
	public boolean isReturning = false;
	public boolean isMacroControl = false;
	public String parameterInfo = "";

	public void getHash(Checksum crc) {
		crc.eat((long) hash);
	}

	public boolean updateHash() {
		Checksum c = new Checksum();

		c.eat(id.getNumber());
		c.eat(id.getVoiceGroup().ordinal());
		c.eat(displayValues);
		c.eat(controlPosition);
		c.eat(bipolar);
		c.eat(selected);
		c.eat(locked);
		c.eat(isBoolean);
		c.eat(changed);
		c.eat(shortName);
		c.eat(longName);
		c.eat(fullNameWithGroup);
		c.eat(fillFromRightEnabled);
		c.eat(disabled);
		c.eat(hidden);
		c.eat(modulation.modulationRange.left);
		c.eat(modulation.modulationRange.right);
		c.eat(modulation.modulationSourceLabel);
		c.eat(modulation.modulationSource.ordinal());
		c.eat(modulation.isModulated);
		c.eat(modulation.isModulateable);
		c.eat(modulation.amountDisplayValues);
		c.eat(modulation.amountLowerBoundDisplayValues);
		c.eat(modulation.amountUpperBoundDisplayValues);
		c.eat(modulation.macroControlDisplayValues);
		c.eat(modulation.lowerClipping);
		c.eat(modulation.upperClipping);
		c.eat(modulation.originalModulationAmountDecoratedString);
		c.eat(modulation.originalModulationPositionDecoratedString);
		c.eat(highlightChanged);
		c.eat(userGivenName);
		c.eat(showContextMenu);
		c.eat(originalParameterValueDecoratedString);
		c.eat(drawHandleOnly);
		c.eat(drawCenterReturnIndicator);
		c.eat(drawZeroReturnIndicator);
		c.eat(isReturning);
		c.eat(isMacroControl);
		c.eat(parameterInfo);

		c.eat(SetupModel.get().systemSettings.highlightChangedParameters.getValue().toString());
		c.eat(SetupModel.get().systemSettings.forceHighlightChangedParameters.getValue().toString());

		long newHash = c.getHash();

		if (hash != newHash) {
			hash = newHash;
			return true;
		}

		return false;
	}
}
