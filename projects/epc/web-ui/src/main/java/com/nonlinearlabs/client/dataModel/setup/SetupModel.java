package com.nonlinearlabs.client.dataModel.setup;

import com.nonlinearlabs.client.Tracer;
import com.nonlinearlabs.client.dataModel.BooleanDataModelEntity;
import com.nonlinearlabs.client.dataModel.EnumDataModelEntity;
import com.nonlinearlabs.client.dataModel.IntegerDataModelEntity;
import com.nonlinearlabs.client.dataModel.RoutingAspectsDataModelEntity;
import com.nonlinearlabs.client.dataModel.StringDataModelEntity;
import com.nonlinearlabs.client.dataModel.ValueDataModelEntity;
import com.nonlinearlabs.client.presenters.DeviceSettings.Pedal;
import com.nonlinearlabs.client.world.maps.parameters.PlayControls.SourcesAndAmounts.Sources.Ribbon;

import org.eclipse.jetty.jndi.local.localContextRoot;

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
		PotTipActive, PotRingActive, PotTipActiveReverse, PotRingActiveReverse, Resistor, ResistorReversed,
		SwitchClosing, SwitchOpening, CV0to5V, CV0To5VAutoRange, OFF, BossEV30, BossFV500L, DoepferFP5, FractalEV2,
		KorgDS1H, KorgEXP2, LeadFootLFX1, MAudioEXP, MoogEP3, RolandDP10, RolandEV5, YamahaFC3A, YamahaFC7
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

	public enum MidiReceiveChannel { 
		None, Omni, CH_1, CH_2, CH_3, CH_4, CH_5, CH_6, CH_7, CH_8, CH_9, CH_10, CH_11, CH_12, CH_13, CH_14, CH_15, CH_16 
	}

	public enum MidiReceiveChannelSplit { 
		None, Omni, CH_1, CH_2, CH_3, CH_4, CH_5, CH_6, CH_7, CH_8, CH_9, CH_10, CH_11, CH_12, CH_13, CH_14, CH_15, CH_16, Common 
	}

	public enum MidiSendChannel {
		None, CH_1, CH_2, CH_3, CH_4, CH_5, CH_6, CH_7, CH_8, CH_9, CH_10, CH_11, CH_12, CH_13, CH_14, CH_15, CH_16 
	}

	public enum MidiSendChannelSplit { 
		None, CH_1, CH_2, CH_3, CH_4, CH_5, CH_6, CH_7, CH_8, CH_9, CH_10, CH_11, CH_12, CH_13, CH_14, CH_15, CH_16, Common
	}

	public enum PedalCCMapping {
		None, CC01, CC02, CC03, CC04, CC05, CC06, CC07, CC08, CC09, CC10, CC11, CC12, CC13, CC14, CC15, CC16, CC17, CC18, CC19, 
		CC20, CC21, CC22, CC23, CC24, CC25, CC26, CC27, CC28, CC29, CC30, CC31, CC64, CC65, CC66, CC67, CC68, CC69
	}

	public enum RibbonCCMapping {
		None, CC01, CC02, CC03, CC04, CC05, CC06, CC07, CC08, CC09, CC10, CC11, CC12, CC13, CC14, CC15, CC16, CC17, CC18, CC19,
		CC20, CC21, CC22, CC23, CC24, CC25, CC26, CC27, CC28, CC29, CC30, CC31
	}

	public enum BenderCCMapping {
		None, Pitchbend, CC01, CC02, CC03, CC04, CC05, CC06, CC07, CC08, CC09, CC10, CC11, CC12, CC13, CC14, CC15, CC16, CC17,
		CC18, CC19, CC20, CC21, CC22, CC23, CC24, CC25, CC26, CC27, CC28, CC29, CC30, CC31
	}

	public enum AftertouchCCMapping {
		None, ChannelPressure, CC01, CC02, CC03, CC04, CC05, CC06, CC07, CC08, CC09, CC10, CC11, CC12, CC13, CC14, CC15, CC16,
		CC17, CC18, CC19, CC20, CC21, CC22, CC23, CC24, CC25, CC26, CC27, CC28, CC29, CC30, CC31, PitchbendUp, PitchbendDown
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

	public class MidiReceiveChannelSetting extends EnumDataModelEntity<MidiReceiveChannel> {
		public MidiReceiveChannelSetting() {
			super(MidiReceiveChannel.class, MidiReceiveChannel.None);
		}

		@Override
		public void fromString(String str) {
			try {
				MidiReceiveChannel c = MidiReceiveChannel.valueOf(str);
				setValue(c);
			} catch(Exception c) {
				Tracer.log("Error: Could not parse Midi Receive Channel value of " + str);				
			}
		}
	}

	public class MidiReceiveChannelSplitSetting extends EnumDataModelEntity<MidiReceiveChannelSplit> {
		public MidiReceiveChannelSplitSetting() {
			super(MidiReceiveChannelSplit.class, MidiReceiveChannelSplit.None);
		}

		@Override
		public void fromString(String str) {
			try {
				MidiReceiveChannelSplit c = MidiReceiveChannelSplit.valueOf(str);
				setValue(c);
			} catch(Exception c) {
				Tracer.log("Error: Could not parse Midi Receive Channel Split value of " + str);				
			}
		}
	}

	public class MidiSendChannelSetting extends EnumDataModelEntity<MidiSendChannel> {
		public MidiSendChannelSetting() {
			super(MidiSendChannel.class, MidiSendChannel.None);
		}

		@Override
		public void fromString(String str) {
			try {
				MidiSendChannel c = MidiSendChannel.valueOf(str);
				setValue(c);
			} catch(Exception c) {
				Tracer.log("Error: Could not parse Midi Send Channel value of " + str);				
			}
		}
	}

	public class MidiSendChannelSplitSetting extends EnumDataModelEntity<MidiSendChannelSplit> {
		public MidiSendChannelSplitSetting() {
			super(MidiSendChannelSplit.class, MidiSendChannelSplit.None);
		}

		@Override
		public void fromString(String str) {
			try {
				MidiSendChannelSplit c = MidiSendChannelSplit.valueOf(str);
				setValue(c);
			} catch(Exception c) {
				Tracer.log("Error: Could not parse Midi Send Channel Split value of " + str);				
			}
		}
	}


	public class MidiReceiveVelocityCurveSetting extends EnumDataModelEntity<VelocityCurve> {
		public MidiReceiveVelocityCurveSetting() {
			super(VelocityCurve.class, VelocityCurve.normal);
		}

		@Override
		public void fromString(String str) {
			try {
				VelocityCurve c = VelocityCurve.valueOf(str);
				setValue(c);
			} catch(Exception c) {
				Tracer.log("Error: Could not parse Midi receive VelocityCurve value of " + str);				
			}
		}
	}

	public class MidiReceiveAftertouchCurve extends EnumDataModelEntity<AftertouchCurve> {
		public MidiReceiveAftertouchCurve() {
			super(AftertouchCurve.class, AftertouchCurve.normal);
		}

		@Override
		public void fromString(String str) {
			try {
				AftertouchCurve c = AftertouchCurve.valueOf(str);
				setValue(c);
			} catch(Exception c) {
				Tracer.log("Error: Could not parse Midi Receive AftertouchCurve value of: " + str);
			}
		}
	}

	public class PedalMappingDataModelEntity extends EnumDataModelEntity<PedalCCMapping> {
		public PedalMappingDataModelEntity() {
			super(PedalCCMapping.class, PedalCCMapping.CC01);
		}


		@Override
		public void fromString(String str) {
			try {
				PedalCCMapping p = PedalCCMapping.valueOf(str);
				setValue(p);
			} catch (Exception e) {
				Tracer.log("WARNING: Could not parse pedal-mapping type value of " + str);
			}
		}
	}

	public class RibbonMappingDataModelEntity extends EnumDataModelEntity<RibbonCCMapping> {
		public RibbonMappingDataModelEntity() {
			super(RibbonCCMapping.class, RibbonCCMapping.CC01);
		}


		@Override
		public void fromString(String str) {
			try {
				RibbonCCMapping p = RibbonCCMapping.valueOf(str);
				setValue(p);
			} catch (Exception e) {
				Tracer.log("WARNING: Could not parse ribbon-mapping type value of " + str);
			}
		}
	}

	public class AftertouchMappingDataModelEntity extends EnumDataModelEntity<AftertouchCCMapping> {
		public AftertouchMappingDataModelEntity() {
			super(AftertouchCCMapping.class, AftertouchCCMapping.ChannelPressure);
		}

		@Override
		public void fromString(String str) {
			try {
				AftertouchCCMapping p = AftertouchCCMapping.valueOf(str);
				setValue(p);
			} catch (Exception e) {
				Tracer.log("WARNING: Could not parse aftertouch-mapping type value of " + str);
			}
		}
	}

	public class PitchbendMappingDataModelEntity extends EnumDataModelEntity<BenderCCMapping> {
		public PitchbendMappingDataModelEntity() {
			super(BenderCCMapping.class, BenderCCMapping.Pitchbend);
		}

		@Override
		public void fromString(String str) {
			try {
				BenderCCMapping p = BenderCCMapping.valueOf(str);
				setValue(p);
			} catch (Exception e) {
				Tracer.log("WARNING: Could not parse bender-mapping type value of " + str);
			}
		}
	}

	class PedalTypeSetting extends EnumDataModelEntity<PedalType> {
		public PedalTypeSetting() {
			super(PedalType.class, PedalType.PotTipActive);
		}

		@Override
		public void fromString(String str) {
			try {
				PedalType p = PedalType.valueOf(str);
				setValue(p);
			} catch (Exception e) {
				Tracer.log("WARNING: Could not parse pedal type value of " + str);
			}
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
		public EnumDataModelEntity<PedalType> pedal1Type = new PedalTypeSetting();
		public EnumDataModelEntity<PedalType> pedal2Type = new PedalTypeSetting();
		public EnumDataModelEntity<PedalType> pedal3Type = new PedalTypeSetting();
		public EnumDataModelEntity<PedalType> pedal4Type = new PedalTypeSetting();
		public BooleanDataModelEntity benderOnPressedKeys = new BooleanDataModelEntity();
		public BooleanDataModelEntity presetDragEnabled = new BooleanDataModelEntity();
		public BooleanDataModelEntity presetGlitchSuppression = new BooleanDataModelEntity();
		public BooleanDataModelEntity syncVoiceGroups = new BooleanDataModelEntity();
		public EnumDataModelEntity<PresetStoreMode> presetStoreModeSetting = createEnumDataModelEntity(
				PresetStoreMode.class, PresetStoreMode.append);
		public ValueDataModelEntity randomizeAmount = new ValueDataModelEntity();
		public ValueDataModelEntity tuneReference = new ValueDataModelEntity();
		public ValueDataModelEntity ribbonRelativeFactor = new ValueDataModelEntity();
        public BooleanDataModelEntity sendPresetAsPlaycontrollerFallback = new BooleanDataModelEntity();
		public BooleanDataModelEntity signalFlowIndication = new BooleanDataModelEntity();
		public StringDataModelEntity ssid = new StringDataModelEntity();
		public ValueDataModelEntity transitionTime = new ValueDataModelEntity();
		public EnumDataModelEntity<VelocityCurve> velocityCurve = createEnumDataModelEntity(VelocityCurve.class,
				VelocityCurve.normal);
		public BooleanDataModelEntity highlightChangedParameters = new BooleanDataModelEntity();
		public BooleanDataModelEntity forceHighlightChangedParameters = new BooleanDataModelEntity();
		public BooleanDataModelEntity crashOnError = new BooleanDataModelEntity();
		public BooleanDataModelEntity syncSplit = new BooleanDataModelEntity();

		//Midi below
		public MidiSendChannelSetting sendChannel = new MidiSendChannelSetting();
		public MidiSendChannelSplitSetting sendChannelSplit = new MidiSendChannelSplitSetting();
		public MidiReceiveChannelSetting receiveChannel = new MidiReceiveChannelSetting();
		public MidiReceiveChannelSplitSetting receiveChannelSplit = new MidiReceiveChannelSplitSetting();
		
		public MidiReceiveVelocityCurveSetting receiveVelocityCurve = new MidiReceiveVelocityCurveSetting();
		public MidiReceiveAftertouchCurve receiveAftertouchCurve = new MidiReceiveAftertouchCurve();

		public PedalMappingDataModelEntity pedal1Mapping = new PedalMappingDataModelEntity();
		public PedalMappingDataModelEntity pedal2Mapping = new PedalMappingDataModelEntity();
		public PedalMappingDataModelEntity pedal3Mapping = new PedalMappingDataModelEntity();
		public PedalMappingDataModelEntity pedal4Mapping = new PedalMappingDataModelEntity();
		public RibbonMappingDataModelEntity ribbon1Mapping = new RibbonMappingDataModelEntity();
		public RibbonMappingDataModelEntity ribbon2Mapping = new RibbonMappingDataModelEntity();
		public AftertouchMappingDataModelEntity aftertouchMapping = new AftertouchMappingDataModelEntity();
		public PitchbendMappingDataModelEntity benderMapping = new PitchbendMappingDataModelEntity();
		public BooleanDataModelEntity highVelocityCC = new BooleanDataModelEntity();
		public BooleanDataModelEntity enable14BitSupport = new BooleanDataModelEntity();
		public BooleanDataModelEntity autoStartRecorder = new BooleanDataModelEntity();

		public RoutingAspectsDataModelEntity routingAspects = new RoutingAspectsDataModelEntity();
		public BooleanDataModelEntity globalLocalEnable = new BooleanDataModelEntity();
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
