package com.nonlinearlabs.client.presenters;

import java.util.LinkedList;
import java.util.function.Function;

import com.nonlinearlabs.client.dataModel.setup.SetupModel;
import com.nonlinearlabs.client.dataModel.setup.SetupModel.SystemSettings;
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
        s.localControllers.onChange(t -> {
            boolean newValue = t == BooleanValues.on;
            if(newValue != settings.localControllers.value != newValue) {
                settings.localControllers.value = newValue;
                notifyClients();
            } 
            return true;
        });

        s.localNotes.onChange(t -> {
            boolean newValue = t == BooleanValues.on;
            if(settings.localNotes.value != newValue) {
                settings.localNotes.value = newValue;   
                notifyClients();
            }
            return true;
        });

        s.localProgramChanges.onChange(t -> {
            boolean newValue = t == BooleanValues.on;
            if(settings.localProgramChanges.value != newValue) {
                settings.localProgramChanges.value = newValue;
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

        s.sendControllers.onChange(t -> {
            boolean newValue = t == BooleanValues.on;
            if(settings.sendControllers.value != newValue) {
                settings.sendControllers.value = newValue;
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

        s.receiveControllers.onChange(t -> {
            boolean newValue = t == BooleanValues.on;
            if(settings.receiveControllers.value != newValue) {
                settings.receiveControllers.value = newValue;
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
