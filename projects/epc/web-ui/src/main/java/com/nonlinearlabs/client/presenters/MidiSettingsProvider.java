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
            settings.localControllers.value = t == BooleanValues.on;
            notifyClients();
            return true;
        });

        s.localNotes.onChange(t -> {
            settings.localNotes.value = t == BooleanValues.on;
            notifyClients();
            return true;
        });

        s.localProgramChanges.onChange(t -> {
            settings.localProgramChanges.value = t == BooleanValues.on;
            notifyClients();
            return true;
        });

        //Send
        s.sendChannel.onChange(t -> {
            settings.sendChannel.selected = t.ordinal();
            notifyClients();
            return true;
        });

        s.sendChannelSplit.onChange(t -> {
            settings.sendChannelSplit.selected = t.ordinal();
            notifyClients();
            return true;
        });

        s.sendControllers.onChange(t -> {
            settings.sendControllers.value = t == BooleanValues.on;
            notifyClients();
            return true;
        });

        s.sendProgramChanges.onChange(t -> {
            settings.sendProgramChanges.value = t == BooleanValues.on;
            notifyClients();
            return true;
        });

        s.sendNotes.onChange(t -> {
            settings.sendNotes.value = t == BooleanValues.on;
            notifyClients();
            return true;
        });

        //Receive
        s.receiveChannel.onChange(t -> {
            settings.receiveChannel.selected = t.ordinal();
            notifyClients();
            return true;
        });

        s.receiveChannelSplit.onChange(t -> {
            settings.receiveChannelSplit.selected = t.ordinal();
            notifyClients();
            return true;
        });

        s.receiveProgramChanges.onChange(t -> {
            settings.receiveProgramChanges.value = t == BooleanValues.on;
            notifyClients();
            return true;
        });

        s.receiveNotes.onChange(t -> {
            settings.receiveNotes.value = t == BooleanValues.on;
            notifyClients();
            return true;
        });

        s.receiveControllers.onChange(t -> {
            settings.receiveControllers.value = t == BooleanValues.on;
            notifyClients();
            return true;
        });

        s.receiveAftertouchCurve.onChange(t -> {
            settings.receiveAftertouchCurve.selected = t.ordinal();
            notifyClients();
            return true;
        });

        s.receiveVelocityCurve.onChange(t -> {
            settings.receiveVelocityCurve.selected = t.ordinal();
            notifyClients();
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
