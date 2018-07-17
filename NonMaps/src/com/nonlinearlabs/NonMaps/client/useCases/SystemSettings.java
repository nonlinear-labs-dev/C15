package com.nonlinearlabs.NonMaps.client.useCases;

import com.nonlinearlabs.NonMaps.client.NonMaps;
import com.nonlinearlabs.NonMaps.client.dataModel.Setup;
import com.nonlinearlabs.NonMaps.client.dataModel.Setup.BenderCurve;
import com.nonlinearlabs.NonMaps.client.dataModel.Setup.BooleanValues;
import com.nonlinearlabs.NonMaps.client.dataModel.Setup.DebugLevel;

public class SystemSettings {
	private static SystemSettings theInstance = new SystemSettings();

	public static SystemSettings get() {
		return theInstance;
	}

	public void setSendPresetAsLPCWriteFallback(BooleanValues v) {
		Setup.get().systemSettings.sendPresetAsLPCFallback.setValue(v);
		NonMaps.get().getServerProxy().setSetting("SendPresetAsLPCWriteFallback", v.name());
	}

	public void setDebugLevel(DebugLevel v) {
		Setup.get().systemSettings.debugLevel.setValue(v);
		NonMaps.get().getServerProxy().setSetting("DebugLevel", v.toString());

	}
	
	public void setPitchBenderRaw(BenderCurve v) {
		Setup.get().systemSettings.benderCurve.setValue(v);
		NonMaps.get().getServerProxy().setSetting("DebugLevel", v.toString());
	}

	public void setKioskMode(BooleanValues v) {
		Setup.get().systemSettings.kioskMode.setValue(v);
		NonMaps.get().getServerProxy().setSetting("KioskMode", v.name());
	}

	public void setBlockingMainThreadIndication(BooleanValues v) {
		Setup.get().systemSettings.indicateBlockedUI.setValue(v);
		NonMaps.get().getServerProxy().setSetting("IndicateBlockedUI", v.name());

	}

	public void setDeviceName(String newName) {
		Setup.get().systemSettings.deviceName.setValue(newName);
		NonMaps.theMaps.getServerProxy().setSetting("DeviceName", newName);
	}

	public void setEditSmoothingTime(double v) {
		Setup.get().systemSettings.editSmoothingTime.setValue(v);
		NonMaps.theMaps.getServerProxy().setSetting("EditSmoothingTime", Double.toString(v));

	}
}
