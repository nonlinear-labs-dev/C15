package com.nonlinearlabs.client.dataModel.setup;

import java.util.HashMap;

import com.google.gwt.xml.client.Node;
import com.nonlinearlabs.client.dataModel.DataModelEntityBase;
import com.nonlinearlabs.client.dataModel.Updater;
import com.nonlinearlabs.client.dataModel.ValueDataModelEntity;
import com.nonlinearlabs.client.dataModel.ValueUpdater;

public class SetupUpdater extends Updater {
	private final HashMap<String, DataModelEntityBase> xmlNodeNameToSetting = createSettingMap();

	public SetupUpdater(Node settings) {
		super(settings);
	}

	private HashMap<String, DataModelEntityBase> createSettingMap() {
		HashMap<String, DataModelEntityBase> m = new HashMap<String, DataModelEntityBase>();
		m.put("AftertouchCurve", SetupModel.get().systemSettings.aftertouchCurve);
		m.put("DirectLoad", SetupModel.get().systemSettings.directLoad);
		m.put("BaseUnitUIMode", SetupModel.get().systemSettings.baseUnitUIMode);
		m.put("BenderCurve", SetupModel.get().systemSettings.benderCurve);
		m.put("DateTimeAdjustment", SetupModel.get().systemSettings.datetimeAdjustment);
		m.put("DebugLevel", SetupModel.get().systemSettings.debugLevel);
		m.put("DeviceName", SetupModel.get().systemSettings.deviceName);
		m.put("EditSmoothingTime", SetupModel.get().systemSettings.editSmoothingTime);
		m.put("EncoderAcceleration", SetupModel.get().systemSettings.encoderAcceleration);
		m.put("IndicateBlockedUI", SetupModel.get().systemSettings.indicateBlockedUI);
		m.put("KioskMode", SetupModel.get().systemSettings.kioskMode);
		m.put("NoteShift", SetupModel.get().systemSettings.noteShift);
		m.put("ParameterEditModeRibbonBehaviour", SetupModel.get().systemSettings.editmodeRibbonBehavior);
		m.put("Passphrase", SetupModel.get().systemSettings.passPhrase);
		m.put("Pedal1Type", SetupModel.get().systemSettings.pedal1Type);
		m.put("Pedal2Type", SetupModel.get().systemSettings.pedal2Type);
		m.put("Pedal3Type", SetupModel.get().systemSettings.pedal3Type);
		m.put("Pedal4Type", SetupModel.get().systemSettings.pedal4Type);
		m.put("PresetDragDropEnabled", SetupModel.get().systemSettings.presetDragEnabled);
		m.put("PresetGlitchSuppression", SetupModel.get().systemSettings.presetGlitchSuppression);
		m.put("PresetStoreModeSetting", SetupModel.get().systemSettings.presetStoreModeSetting);
		m.put("RibbonRelFactor", SetupModel.get().systemSettings.ribbonRelativeFactor);
        m.put("SendPresetAsLPCWriteFallback", SetupModel.get().systemSettings.sendPresetAsPlaycontrollerFallback);
		m.put("SignalFlowIndication", SetupModel.get().systemSettings.signalFlowIndication);
		m.put("SSID", SetupModel.get().systemSettings.ssid);
		m.put("TransitionTime", SetupModel.get().systemSettings.transitionTime);
		m.put("TuneReference", SetupModel.get().systemSettings.tuneReference);
		m.put("VelocityCurve", SetupModel.get().systemSettings.velocityCurve);
		m.put("RandomizeAmount", SetupModel.get().systemSettings.randomizeAmount);
		m.put("HighlightChangedParameters", SetupModel.get().systemSettings.highlightChangedParameters);
		m.put("ForceHighlightChangedParameters", SetupModel.get().systemSettings.forceHighlightChangedParameters);
		m.put("CrashOnError", SetupModel.get().systemSettings.crashOnError);
		m.put("SyncVoiceGroups", SetupModel.get().systemSettings.syncVoiceGroups);
		m.put("TotalRAM", DeviceInformation.get().totalRam);
		m.put("UsedRAM", DeviceInformation.get().usedRam);
		m.put("SyncSplit", SetupModel.get().systemSettings.syncSplit);
		m.put("ReceiveChannel", SetupModel.get().systemSettings.receiveChannel);
		m.put("ReceiveChannelSplit", SetupModel.get().systemSettings.receiveChannelSplit);
		m.put("ReceiveAftertouchCurve", SetupModel.get().systemSettings.receiveAftertouchCurve);
		m.put("ReceiveVelocityCurve", SetupModel.get().systemSettings.receiveVelocityCurve);
		m.put("SendChannel", SetupModel.get().systemSettings.sendChannel);
		m.put("SendChannelSplit", SetupModel.get().systemSettings.sendChannelSplit);
		m.put("Pedal1Mapping", SetupModel.get().systemSettings.pedal1Mapping);
		m.put("Pedal2Mapping", SetupModel.get().systemSettings.pedal2Mapping);
		m.put("Pedal3Mapping", SetupModel.get().systemSettings.pedal3Mapping);
		m.put("Pedal4Mapping", SetupModel.get().systemSettings.pedal4Mapping);
		m.put("Ribbon1Mapping", SetupModel.get().systemSettings.ribbon1Mapping);
		m.put("Ribbon2Mapping", SetupModel.get().systemSettings.ribbon2Mapping);
		m.put("BenderMapping", SetupModel.get().systemSettings.benderMapping);
		m.put("HighVeloCC", SetupModel.get().systemSettings.highVelocityCC);
		m.put("HighResCC", SetupModel.get().systemSettings.enable14BitSupport);
		m.put("AftertouchMapping", SetupModel.get().systemSettings.aftertouchMapping);
		m.put("AutoStartRecorder", SetupModel.get().systemSettings.autoStartRecorder);
		m.put("RoutingSettings", SetupModel.get().systemSettings.routingAspects);
		m.put("GlobalLocalEnable", SetupModel.get().systemSettings.globalLocalEnable);
		return m;
	}

	public void doUpdate() {
		Node setting = root.getFirstChild();

		while (setting != null) {
			if (setting.getNodeType() == Node.ELEMENT_NODE) {
				Node valueNode = getChild(setting, "value");

				if (didChange(valueNode)) {
					DataModelEntityBase s = findSettingFromTagName(setting.getNodeName());
					if (s != null) {
						s.fromString(getText(valueNode));

						if (s instanceof ValueDataModelEntity) {
							ValueUpdater u = new ValueUpdater(setting, (ValueDataModelEntity) s);
							u.doUpdate();
						}
					}
				}
			}
			setting = setting.getNextSibling();
		}
	}

	private DataModelEntityBase findSettingFromTagName(String nodeName) {
		return xmlNodeNameToSetting.get(nodeName);
	}
}
