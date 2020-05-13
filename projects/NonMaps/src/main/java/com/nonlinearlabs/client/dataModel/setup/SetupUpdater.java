package com.nonlinearlabs.client.dataModel.setup;

import java.util.HashMap;

import com.google.gwt.core.client.GWT;
import com.google.gwt.xml.client.Node;
import com.nonlinearlabs.client.dataModel.DataModelEntityBase;
import com.nonlinearlabs.client.dataModel.EnumDataModelEntity;
import com.nonlinearlabs.client.dataModel.Updater;
import com.nonlinearlabs.client.dataModel.ValueDataModelEntity;
import com.nonlinearlabs.client.dataModel.ValueUpdater;
import com.nonlinearlabs.client.dataModel.setup.SetupModel.PedalType;

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
		m.put("PitchbendOnPressedKeys", SetupModel.get().systemSettings.benderOnPressedKeys);
		m.put("PresetDragDropEnabled", SetupModel.get().systemSettings.presetDragEnabled);
		m.put("PresetGlitchSuppression", SetupModel.get().systemSettings.presetGlitchSuppression);
		m.put("PresetStoreModeSetting", SetupModel.get().systemSettings.presetStoreModeSetting);
		m.put("RibbonRelFactor", SetupModel.get().systemSettings.ribbonRelativeFactor);
		m.put("SendPresetAsLPCWriteFallback", SetupModel.get().systemSettings.sendPresetAsLPCFallback);
		m.put("SignalFlowIndication", SetupModel.get().systemSettings.signalFlowIndication);
		m.put("SSID", SetupModel.get().systemSettings.ssid);
		m.put("TransitionTime", SetupModel.get().systemSettings.transitionTime);
		m.put("TuneReference", SetupModel.get().systemSettings.tuneReference);
		m.put("VelocityCurve", SetupModel.get().systemSettings.velocityCurve);
		m.put("RandomizeAmount", SetupModel.get().systemSettings.randomizeAmount);
		m.put("BenderRampBypass", SetupModel.get().systemSettings.benderRampBypass);
		m.put("HighlightChangedParameters", SetupModel.get().systemSettings.highlightChangedParameters);
		m.put("ForceHighlightChangedParameters", SetupModel.get().systemSettings.forceHighlightChangedParameters);
		m.put("CrashOnError", SetupModel.get().systemSettings.crashOnError);

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
						
						if(setting.getNodeName().matches("Pedal\\dType") && s instanceof EnumDataModelEntity<?>) {
							EnumDataModelEntity<PedalType> e = (EnumDataModelEntity<PedalType>)s;
							e.fromInt(Integer.parseInt(getText(valueNode)));
						} else {
							s.fromString(getText(valueNode));
						}

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
