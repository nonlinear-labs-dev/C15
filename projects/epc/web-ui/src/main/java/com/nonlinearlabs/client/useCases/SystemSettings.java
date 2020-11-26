package com.nonlinearlabs.client.useCases;

import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.dataModel.setup.SetupModel;
import com.nonlinearlabs.client.dataModel.setup.SetupModel.AftertouchCurve;
import com.nonlinearlabs.client.dataModel.setup.SetupModel.BenderCurve;
import com.nonlinearlabs.client.dataModel.setup.SetupModel.BooleanValues;
import com.nonlinearlabs.client.dataModel.setup.SetupModel.DebugLevel;
import com.nonlinearlabs.client.dataModel.setup.SetupModel.PedalType;
import com.nonlinearlabs.client.dataModel.setup.SetupModel.VelocityCurve;

public class SystemSettings {
	private static SystemSettings theInstance = new SystemSettings();

	public static SystemSettings get() {
		return theInstance;
	}

    public void setSendPresetAsPlaycontrollerWriteFallback(BooleanValues v) {
        SetupModel.get().systemSettings.sendPresetAsPlaycontrollerFallback.setValue(v);
		NonMaps.get().getServerProxy().setSetting("SendPresetAsLPCWriteFallback", netify(v.name()));
	}

	private String netify(String name) {
		name = name.toLowerCase();
		return name.replace("_", "-");
	}

	public void setDebugLevel(DebugLevel v) {
		SetupModel.get().systemSettings.debugLevel.setValue(v);
		NonMaps.get().getServerProxy().setSetting("DebugLevel", netify(v.name()));
	}

	public void setKioskMode(BooleanValues v) {
		SetupModel.get().systemSettings.kioskMode.setValue(v);
		NonMaps.get().getServerProxy().setSetting("KioskMode", netify(v.name()));
	}

	public void setBlockingMainThreadIndication(BooleanValues v) {
		SetupModel.get().systemSettings.indicateBlockedUI.setValue(v);
		NonMaps.get().getServerProxy().setSetting("IndicateBlockedUI", netify(v.name()));
	}

	public void setDeviceName(String newName) {
		SetupModel.get().systemSettings.deviceName.setValue(newName);
		NonMaps.theMaps.getServerProxy().setSetting("DeviceName", newName);
	}

	public void setEditSmoothingTime(double v) {
		SetupModel.get().systemSettings.editSmoothingTime.value.setValue(v);
		NonMaps.theMaps.getServerProxy().setSetting("EditSmoothingTime", Double.toString(v));
	}

	public void setVelocityCurve(VelocityCurve c) {
		SetupModel.get().systemSettings.velocityCurve.setValue(c);
		NonMaps.theMaps.getServerProxy().setSetting("VelocityCurve", netify(c.name()));
	}

	public void setSyncParts(BooleanValues v) {
		SetupModel.get().systemSettings.syncVoiceGroups.setValue(v);
		NonMaps.theMaps.getServerProxy().setSetting("SyncVoiceGroups", netify(v.name()));
	}

	public void setAftertouchCurve(AftertouchCurve c) {
		SetupModel.get().systemSettings.aftertouchCurve.setValue(c);
		NonMaps.theMaps.getServerProxy().setSetting("AftertouchCurve", netify(c.name()));
	}

	public void setBenderCurve(BenderCurve c) {
		SetupModel.get().systemSettings.benderCurve.setValue(c);
		NonMaps.theMaps.getServerProxy().setSetting("BenderCurve", netify(c.name()));
	}

	public void setPedal1Type(PedalType c) {
		SetupModel.get().systemSettings.pedal1Type.setValue(c);
		NonMaps.theMaps.getServerProxy().setSetting("Pedal1Type", c.name());
	}

	public void setPedal2Type(PedalType c) {
		SetupModel.get().systemSettings.pedal2Type.setValue(c);
		NonMaps.theMaps.getServerProxy().setSetting("Pedal2Type", c.name());
	}

	public void setPedal3Type(PedalType c) {
		SetupModel.get().systemSettings.pedal3Type.setValue(c);
		NonMaps.theMaps.getServerProxy().setSetting("Pedal3Type", c.name());
	}

	public void setPedal4Type(PedalType c) {
		SetupModel.get().systemSettings.pedal4Type.setValue(c);
		NonMaps.theMaps.getServerProxy().setSetting("Pedal4Type", c.name());
	}

	public void setPresetGlitchSuppression(BooleanValues c) {
		SetupModel.get().systemSettings.presetGlitchSuppression.setValue(c);
		NonMaps.theMaps.getServerProxy().setSetting("PresetGlitchSuppression", netify(c.name()));
	}

	public void setHighlightChangedParameters(BooleanValues c) {
		SetupModel.get().systemSettings.highlightChangedParameters.setValue(c);
		NonMaps.theMaps.getServerProxy().setSetting("HighlightChangedParameters", netify(c.name()));
	}

	public void setCrashOnError(BooleanValues c) {
		SetupModel.get().systemSettings.crashOnError.setValue(c);
		NonMaps.theMaps.getServerProxy().setSetting("CrashOnError", netify(c.name()));
	}

	public void setRandomizeAmount(double amount) {
		SetupModel.get().systemSettings.randomizeAmount.value.setValue(amount);
		NonMaps.theMaps.getServerProxy().setRandomAmount(amount);
	}

	public void setTransitionTime(double v) {
		SetupModel.get().systemSettings.transitionTime.value.setValue(v);
		NonMaps.theMaps.getServerProxy().setTransitionTime(v);
	}

	public void setTuneReference(double v) {
		SetupModel.get().systemSettings.tuneReference.value.setValue(v);
		NonMaps.theMaps.getServerProxy().setSetting("TuneReference", Double.toString(v));
	}

	public void resetInitSound() {
		NonMaps.theMaps.getServerProxy().resetInit();
	}

	public void storeInitSound() {
		NonMaps.theMaps.getServerProxy().storeAsInit();
	}

	public void setExternalMidi(boolean midiEnable) {
		SetupModel.get().systemSettings.externalMidi.setValue(midiEnable);
		NonMaps.theMaps.getServerProxy().setSetting("ExternalMidi", Boolean.toString(midiEnable));
	}
}
