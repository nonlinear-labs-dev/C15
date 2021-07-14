package com.nonlinearlabs.client.presenters;

import java.util.LinkedList;
import java.util.function.Function;

import com.nonlinearlabs.client.dataModel.setup.SetupModel;
import com.nonlinearlabs.client.dataModel.setup.SetupModel.SystemSettings;
import com.nonlinearlabs.client.presenters.MidiSettings.HWSourceEnableSettings;

import org.apache.xpath.operations.Bool;

import com.nonlinearlabs.client.dataModel.setup.SetupModel.BooleanValues;

public class MidiSettingsProvider {
	public static MidiSettingsProvider theInstance = new MidiSettingsProvider();

	public static MidiSettingsProvider get() {
		return theInstance;
	}

	private LinkedList<Function<MidiSettings, Boolean>> clients = new LinkedList<Function<MidiSettings, Boolean>>();
	private MidiSettings settings = new MidiSettings();

	private MidiSettingsProvider() {
		SystemSettings s = SetupModel.get().systemSettings;

        //Local
        s.localNotes.onChange(t -> {
            boolean newValue = t == BooleanValues.on;
            if(settings.localNotes.value != newValue) {
                settings.localNotes.value = newValue;   
                notifyClients();
            }
            return true;
        });

        //Send
        s.sendChannel.onChange(t -> {
            int newValue = t.ordinal();
            if(settings.sendChannel.selected != newValue) {
                settings.sendChannel.selected = t.ordinal();
                notifyClients();
            }
            return true;
        });

        s.sendChannelSplit.onChange(t -> {
            int newValue = t.ordinal();
            if(settings.sendChannelSplit.selected != newValue) {
                settings.sendChannelSplit.selected = newValue;
                notifyClients();
            }
            return true;
        });

        s.sendProgramChanges.onChange(t -> {
            boolean newValue = t == BooleanValues.on;
            if(settings.sendProgramChanges.value != newValue) {
                settings.sendProgramChanges.value = newValue;
                notifyClients();
            }
            return true;
        });

        s.sendNotes.onChange(t -> {
            boolean newValue = t == BooleanValues.on;
            if(settings.sendNotes.value != newValue) {
                settings.sendNotes.value = newValue;
                notifyClients();
            }
            return true;
        });

        //Receive
        s.receiveChannel.onChange(t -> {
            int newValue = t.ordinal();
            if(settings.receiveChannel.selected != newValue) {
                settings.receiveChannel.selected = newValue;
                notifyClients();
            }
            return true;
        });

        s.receiveChannelSplit.onChange(t -> {
            int newValue = t.ordinal();
            if(settings.receiveChannelSplit.selected != newValue) {
                settings.receiveChannelSplit.selected = newValue;
                notifyClients();
            }
            return true;
        });

        s.receiveProgramChanges.onChange(t -> {
            boolean newValue = t == BooleanValues.on;
            if(settings.receiveProgramChanges.value != newValue) {
                settings.receiveProgramChanges.value = newValue;
                notifyClients();
            }
            return true;
        });

        s.receiveNotes.onChange(t -> {
            boolean newValue = t == BooleanValues.on;
            if(settings.receiveNotes.value != newValue) {
                settings.receiveNotes.value = newValue;
                notifyClients();
            }
            return true;
        });

        s.receiveAftertouchCurve.onChange(t -> {
            int newValue = t.ordinal();
            if(settings.receiveAftertouchCurve.selected != newValue) {
                settings.receiveAftertouchCurve.selected = newValue;   
                notifyClients();
            }
            return true;
        });

        s.receiveVelocityCurve.onChange(t -> {
            int newValue = t.ordinal();
            if(settings.receiveVelocityCurve.selected != newValue) {
                settings.receiveVelocityCurve.selected = newValue;
                notifyClients();
            }
            return true;
        });

        s.pedal1Mapping.onChange(t -> {
            int newValue = t.ordinal();
            if(settings.pedalMapping1.selected != newValue) {
                settings.pedalMapping1.selected = newValue;
                notifyClients();
            }
            return true;
        });

        s.pedal2Mapping.onChange(t -> {
            int newValue = t.ordinal();
            if(settings.pedalMapping2.selected != newValue) {
                settings.pedalMapping2.selected = newValue;
                notifyClients();
            }
            return true;
        });

        s.pedal3Mapping.onChange(t -> {
            int newValue = t.ordinal();
            if(settings.pedalMapping3.selected != newValue) {
                settings.pedalMapping3.selected = newValue;
                notifyClients();
            }
            return true;
        });

        s.pedal4Mapping.onChange(t -> {
            int newValue = t.ordinal();
            if(settings.pedalMapping4.selected != newValue) {
                settings.pedalMapping4.selected = newValue;
                notifyClients();
            }
            return true;
        });

        s.ribbon1Mapping.onChange(t -> {
            int newValue = t.ordinal();
            if(settings.ribbonMapping1.selected != newValue) {
                settings.ribbonMapping1.selected = newValue;
                notifyClients();
            }
            return true;
        });

        s.ribbon2Mapping.onChange(t -> {
            int newValue = t.ordinal();
            if(settings.ribbonMapping2.selected != newValue) {
                settings.ribbonMapping2.selected = newValue;
                notifyClients();
            }
            return true;
        });

        s.benderMapping.onChange(t -> {
            int newValue = t.ordinal();
            if(settings.benderMapping.selected != newValue) {
                settings.benderMapping.selected = newValue;
                notifyClients();
            }
            return true;
        });

        s.aftertouchMapping.onChange(t -> {
            int newValue = t.ordinal();
            if(settings.aftertouchMapping.selected != newValue) {
                settings.aftertouchMapping.selected = newValue;
                notifyClients();
            }
            return true;
        });

        s.highVelocityCC.onChange(t -> {
            boolean val = t.equals(BooleanValues.on);
            if(settings.highVelocityCC.value != val) {
                settings.highVelocityCC.value = val;
                notifyClients();
            }
            return true;
        });
	
        s.enable14BitSupport.onChange(t -> {
            boolean val = t.equals(BooleanValues.on);
            if(settings.enable14BitCC.value != val) {
                settings.enable14BitCC.value = val;
                notifyClients();
            }
            return true;
        });

        s.autoStartRecorder.onChange(t -> {
            boolean val = t.equals(BooleanValues.on);
            if(settings.autoStartRecorder.value != val) {
                settings.autoStartRecorder.value = val;
                notifyClients();
            }
            return true;
        });

        s.hwSourceMapping.onChange(t -> {
            HWSourceEnableSettings d = MidiSettings.convert(t);
            if(d != settings.hwControlEnables) {
                settings.hwControlEnables = d;
                notifyClients();
            }
            return true;
        });
    }

	protected void notifyClients() {
		clients.removeIf(listener -> !listener.apply(settings));
	}

	public void register(Function<MidiSettings, Boolean> cb) {
		clients.add(cb);
		cb.apply(settings);
	}

	public MidiSettings getSettings() {
		return settings;
	}
}
