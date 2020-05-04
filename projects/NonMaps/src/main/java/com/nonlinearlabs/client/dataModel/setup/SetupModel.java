package com.nonlinearlabs.client.dataModel.setup;

import com.nonlinearlabs.client.dataModel.BooleanDataModelEntity;
import com.nonlinearlabs.client.dataModel.EnumDataModelEntity;
import com.nonlinearlabs.client.dataModel.IntegerDataModelEntity;
import com.nonlinearlabs.client.dataModel.StringDataModelEntity;
import com.nonlinearlabs.client.dataModel.ValueDataModelEntity;

public class SetupModel {

	private static SetupModel theInstance = new SetupModel();

	static public SetupModel get() {
		return theInstance;
	}

	public enum AftertouchCurve {
		soft, normal, hard
	};

	public enum BaseUnitUIMode {
		play, parameter_edit, banks, presets;
	}

	public enum BenderCurve {
		soft, normal, hard
	}

	public enum DebugLevel {
		silent, error, warning, info, debug, gassy
	}

	public enum EditModeRibbonBehaviour {
		relative, absolute
	}

	public enum PedalType {
		pot_tip_active, pot_ring_active, switch_closing, switch_opening
	}

	public enum PresetStoreMode {
		append, overwrite, insert
	}

	public enum VelocityCurve {
		very_soft, soft, normal, hard, very_hard
	}

	public enum BooleanValues {
		on, off
	}

	public enum SelectionAutoScroll {
		off, parameter, preset, parameter_and_preset
	}

	public enum EditParameter {
		always, if_selected, never
	}

	public enum DisplayScaling {
		percent_50, percent_75, percent_100, percent_125, percent_150
	}

	public enum StripeBrightness {
		off, percent_10, percent_25, percent_50
	}

	private <T extends Enum<T>> EnumDataModelEntity<T> createEnumDataModelEntity(Class<T> c, T def) {
		return new EnumDataModelEntity<T>(c, def);
	}

	public class StripeBrightnessSetting extends EnumDataModelEntity<StripeBrightness> {
		public StripeBrightnessSetting() {
			super(StripeBrightness.class, StripeBrightness.percent_25);
		}

		public double toPercent() {
			switch (getValue()) {
			case off:
				return 0;

			case percent_10:
				return 10;

			case percent_25:
				return 25;

			case percent_50:
				return 50;
			}
			return 0;
		}
	}

	public class SystemSettings {
		public BooleanDataModelEntity directLoad = new BooleanDataModelEntity();
		public EnumDataModelEntity<AftertouchCurve> aftertouchCurve = createEnumDataModelEntity(AftertouchCurve.class,
				AftertouchCurve.normal);
		public EnumDataModelEntity<BaseUnitUIMode> baseUnitUIMode = createEnumDataModelEntity(BaseUnitUIMode.class,
				BaseUnitUIMode.parameter_edit);
		public EnumDataModelEntity<BenderCurve> benderCurve = createEnumDataModelEntity(BenderCurve.class,
				BenderCurve.normal);
		public IntegerDataModelEntity datetimeAdjustment = new IntegerDataModelEntity();
		public EnumDataModelEntity<DebugLevel> debugLevel = createEnumDataModelEntity(DebugLevel.class,
				DebugLevel.warning);
		public StringDataModelEntity deviceName = new StringDataModelEntity();
		public ValueDataModelEntity editSmoothingTime = new ValueDataModelEntity();
		public ValueDataModelEntity encoderAcceleration = new ValueDataModelEntity();
		public BooleanDataModelEntity indicateBlockedUI = new BooleanDataModelEntity();
		public BooleanDataModelEntity kioskMode = new BooleanDataModelEntity();
		public IntegerDataModelEntity noteShift = new IntegerDataModelEntity();
		public EnumDataModelEntity<EditModeRibbonBehaviour> editmodeRibbonBehavior = createEnumDataModelEntity(
				EditModeRibbonBehaviour.class, EditModeRibbonBehaviour.absolute);
		public StringDataModelEntity passPhrase = new StringDataModelEntity();
		public EnumDataModelEntity<PedalType> pedal1Type = createEnumDataModelEntity(PedalType.class,
				PedalType.pot_tip_active);
		public EnumDataModelEntity<PedalType> pedal2Type = createEnumDataModelEntity(PedalType.class,
				PedalType.pot_tip_active);
		public EnumDataModelEntity<PedalType> pedal3Type = createEnumDataModelEntity(PedalType.class,
				PedalType.pot_tip_active);
		public EnumDataModelEntity<PedalType> pedal4Type = createEnumDataModelEntity(PedalType.class,
				PedalType.pot_tip_active);
		public BooleanDataModelEntity benderOnPressedKeys = new BooleanDataModelEntity();
		public BooleanDataModelEntity presetDragEnabled = new BooleanDataModelEntity();
		public BooleanDataModelEntity presetGlitchSuppression = new BooleanDataModelEntity();
		public EnumDataModelEntity<PresetStoreMode> presetStoreModeSetting = createEnumDataModelEntity(
				PresetStoreMode.class, PresetStoreMode.append);
		public ValueDataModelEntity randomizeAmount = new ValueDataModelEntity();
		public ValueDataModelEntity tuneReference = new ValueDataModelEntity();
		public ValueDataModelEntity ribbonRelativeFactor = new ValueDataModelEntity();
		public BooleanDataModelEntity sendPresetAsLPCFallback = new BooleanDataModelEntity();
		public BooleanDataModelEntity signalFlowIndication = new BooleanDataModelEntity();
		public StringDataModelEntity ssid = new StringDataModelEntity();
		public ValueDataModelEntity transitionTime = new ValueDataModelEntity();
		public EnumDataModelEntity<VelocityCurve> velocityCurve = createEnumDataModelEntity(VelocityCurve.class,
				VelocityCurve.normal);
		public BooleanDataModelEntity benderRampBypass = new BooleanDataModelEntity();
		public BooleanDataModelEntity highlightChangedParameters = new BooleanDataModelEntity();
		public BooleanDataModelEntity forceHighlightChangedParameters = new BooleanDataModelEntity();
		public BooleanDataModelEntity crashOnError = new BooleanDataModelEntity();
	};

	public class LocalSettings {
		public EnumDataModelEntity<SelectionAutoScroll> selectionAutoScroll = createEnumDataModelEntity(
				SelectionAutoScroll.class, SelectionAutoScroll.parameter_and_preset);
		public EnumDataModelEntity<EditParameter> editParameter = createEnumDataModelEntity(EditParameter.class,
				EditParameter.if_selected);
		public BooleanDataModelEntity contextMenus = new BooleanDataModelEntity();
		public BooleanDataModelEntity presetDragDrop = new BooleanDataModelEntity();
		public EnumDataModelEntity<DisplayScaling> displayScaling = createEnumDataModelEntity(DisplayScaling.class,
				DisplayScaling.percent_100);
		public StripeBrightnessSetting stripeBrightness = new StripeBrightnessSetting();
		public BooleanDataModelEntity bitmapCache = new BooleanDataModelEntity();
		public BooleanDataModelEntity showDeveloperOptions = new BooleanDataModelEntity();
	}

	public SystemSettings systemSettings = new SystemSettings();
	public LocalSettings localSettings = new LocalSettings();
}
