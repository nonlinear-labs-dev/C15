package com.nonlinearlabs.client.presenters;

import java.util.LinkedList;
import java.util.function.Function;

import com.google.gwt.core.client.Scheduler;
import com.google.gwt.core.client.Scheduler.ScheduledCommand;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.VoiceGroup;
import com.nonlinearlabs.client.dataModel.editBuffer.ParameterId;
import com.nonlinearlabs.client.dataModel.editBuffer.PedalParameterModel;
import com.nonlinearlabs.client.dataModel.setup.SetupModel;
import com.nonlinearlabs.client.dataModel.setup.SetupModel.BooleanValues;
import com.nonlinearlabs.client.presenters.DeviceSettings.Pedal;
import com.nonlinearlabs.client.world.Control;

public class DeviceSettingsProvider {
	public static DeviceSettingsProvider theInstance = new DeviceSettingsProvider();

	public static DeviceSettingsProvider get() {
		return theInstance;
	}

	private LinkedList<Function<DeviceSettings, Boolean>> clients = new LinkedList<Function<DeviceSettings, Boolean>>();
	private DeviceSettings settings = new DeviceSettings();

	public DeviceSettings getPresenter() {
		return settings;
	}

	private DeviceSettingsProvider() {
		SetupModel.get().systemSettings.velocityCurve.onChange(t -> {
			settings.velocityCurve.selected = t.ordinal();
			notifyClients();
			return true;
		});

		SetupModel.get().systemSettings.aftertouchCurve.onChange(t -> {
			settings.aftertouchCurve.selected = t.ordinal();
			notifyClients();
			return true;
		});

		SetupModel.get().systemSettings.benderCurve.onChange(t -> {
			settings.benderCurve.selected = t.ordinal();
			notifyClients();
			return true;
		});

		SetupModel.get().systemSettings.pedal1Type.onChange(t -> {
			settings.pedal1.selected = t.ordinal();
			notifyClients();
			return true;
		});

		SetupModel.get().systemSettings.pedal2Type.onChange(t -> {
			settings.pedal2.selected = t.ordinal();
			notifyClients();
			return true;
		});

		SetupModel.get().systemSettings.pedal3Type.onChange(t -> {
			settings.pedal3.selected = t.ordinal();
			notifyClients();
			return true;
		});

		SetupModel.get().systemSettings.pedal4Type.onChange(t -> {
			settings.pedal4.selected = t.ordinal();
			notifyClients();
			return true;
		});

		SetupModel.get().systemSettings.presetGlitchSuppression.onChange(t -> {
			settings.presetGlitchSuppression.value = (t == BooleanValues.on);
			notifyClients();
			return true;
		});

		SetupModel.get().systemSettings.syncVoiceGroups.onChange(t -> {
			settings.syncParts.value = (t == BooleanValues.on);
			notifyClients();
			return true;
		});

		SetupModel.get().systemSettings.editSmoothingTime.onChange(t -> {
			double v = t.value.getValue();
			settings.editSmoothingTime.sliderPosition = v;
			settings.editSmoothingTime.displayValue = Stringizers.get()
					.stringize(SetupModel.get().systemSettings.editSmoothingTime.metaData.scaling.getValue(), v);
			notifyClients();
			return true;
		});

		SetupModel.get().systemSettings.deviceName.onChange(t -> {
			settings.deviceName = t;
			notifyClients();
			return true;
		});

		SetupModel.get().systemSettings.highlightChangedParameters.onChange(t -> {
			settings.highlightChangedParameters.value = t == BooleanValues.on;
			notifyClients();
			return true;
		});

		connectToPedal(254, settings.pedal1);
		connectToPedal(259, settings.pedal2);
		connectToPedal(264, settings.pedal3);
		connectToPedal(269, settings.pedal4);

		SetupModel.get().systemSettings.randomizeAmount.onChange(t -> {
			settings.randomizeAmountValue = t.value.getValue();
			settings.randomizeAmountDisplayString = t.getDecoratedValue(true, true);
			notifyClients();
			return true;
		});

		SetupModel.get().systemSettings.transitionTime.onChange(t -> {
			settings.transitionTimeValue = t.value.getValue();
			settings.transitionTimeDisplayString = t.getDecoratedValue(true, true);
			notifyClients();
			return true;
		});

		SetupModel.get().systemSettings.tuneReference.onChange(t -> {
			settings.tuneReferenceValue = t.value.getValue();
			settings.tuneReferenceDisplayString = t.getDecoratedValue(true, true);
			notifyClients();
			return true;
		});

		SetupModel.get().systemSettings.syncSplit.onChange(t -> {
			settings.syncSplitsEnabled = t.equals(BooleanValues.on);
			notifyClients();
			return true;
		});

		SetupModel.get().systemSettings.externalMidi.onChange(t -> {
			settings.externalMidiEnabled = t.equals(BooleanValues.on);
			notifyClients();
			return true;
		});
	}

	public void connectToPedal(int id, Pedal target) {
		PedalParameterModel srcPedal = (PedalParameterModel) EditBufferModel.get()
				.getParameter(new ParameterId(id, VoiceGroup.Global));
		srcPedal.value.onChange(t -> {
			double v = t.value.getValue();
			target.displayValue = Stringizers.get().stringize(srcPedal.value.metaData.scaling.getValue(), v);
			target.sliderPosition = v;
			notifyClients();
			return true;
		});
	}

	boolean scheduled = false;

	protected void notifyClients() {
		if (!scheduled) {
			scheduled = true;
			Scheduler.get().scheduleDeferred(new ScheduledCommand() {

				@Override
				public void execute() {
					scheduled = false;
					clients.removeIf(listener -> !listener.apply(settings));
				}
			});
		}
	}

	public void register(Function<DeviceSettings, Boolean> cb) {
		clients.add(cb);
		cb.apply(settings);
	}
}
