package com.nonlinearlabs.client.presenters;

import com.nonlinearlabs.client.dataModel.Notifier;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.VoiceGroup;
import com.nonlinearlabs.client.dataModel.editBuffer.ParameterId;
import com.nonlinearlabs.client.dataModel.editBuffer.PedalParameterModel;
import com.nonlinearlabs.client.dataModel.setup.SetupModel;
import com.nonlinearlabs.client.dataModel.setup.SetupModel.BooleanValues;
import com.nonlinearlabs.client.presenters.DeviceSettings.Pedal;

public class DeviceSettingsProvider extends Notifier<DeviceSettings> {
	public static DeviceSettingsProvider theInstance = new DeviceSettingsProvider();

	public static DeviceSettingsProvider get() {
		return theInstance;
	}

	private DeviceSettings settings = new DeviceSettings();

	public DeviceSettings getPresenter() {
		return settings;
	}

	private DeviceSettingsProvider() {
		SetupModel.get().systemSettings.velocityCurve.onChange(t -> {
			settings.velocityCurve.selected = t.ordinal();
			notifyChanges();
			return true;
		});

		SetupModel.get().systemSettings.aftertouchCurve.onChange(t -> {
			settings.aftertouchCurve.selected = t.ordinal();
			notifyChanges();
			return true;
		});

		SetupModel.get().systemSettings.benderCurve.onChange(t -> {
			settings.benderCurve.selected = t.ordinal();
			notifyChanges();
			return true;
		});

		SetupModel.get().systemSettings.pedal1Type.onChange(t -> {
			settings.pedal1.selected = t.ordinal();
			notifyChanges();
			return true;
		});

		SetupModel.get().systemSettings.pedal2Type.onChange(t -> {
			settings.pedal2.selected = t.ordinal();
			notifyChanges();
			return true;
		});

		SetupModel.get().systemSettings.pedal3Type.onChange(t -> {
			settings.pedal3.selected = t.ordinal();
			notifyChanges();
			return true;
		});

		SetupModel.get().systemSettings.pedal4Type.onChange(t -> {
			settings.pedal4.selected = t.ordinal();
			notifyChanges();
			return true;
		});

		SetupModel.get().systemSettings.presetGlitchSuppression.onChange(t -> {
			settings.presetGlitchSuppression.value = (t == BooleanValues.on);
			notifyChanges();
			return true;
		});

		SetupModel.get().systemSettings.syncVoiceGroups.onChange(t -> {
			settings.syncParts.value = (t == BooleanValues.on);
			notifyChanges();
			return true;
		});

		SetupModel.get().systemSettings.editSmoothingTime.onChange(t -> {
			double v = t.value.getValue();
			settings.editSmoothingTime.sliderPosition = v;
			settings.editSmoothingTime.displayValue = Stringizers.get()
					.stringize(SetupModel.get().systemSettings.editSmoothingTime.metaData.scaling.getValue(), v);
			notifyChanges();
			return true;
		});

		SetupModel.get().systemSettings.deviceName.onChange(t -> {
			settings.deviceName = t;
			notifyChanges();
			return true;
		});

		SetupModel.get().systemSettings.highlightChangedParameters.onChange(t -> {
			settings.highlightChangedParameters.value = t == BooleanValues.on;
			notifyChanges();
			return true;
		});

		SetupModel.get().systemSettings.presetStoreMode.onChange(t -> {
			if (settings.presetStoreMode != t) {
				settings.presetStoreMode = t;
				notifyChanges();
			}
			return true;
		});

		connectToPedal(254, settings.pedal1);
		connectToPedal(259, settings.pedal2);
		connectToPedal(264, settings.pedal3);
		connectToPedal(269, settings.pedal4);

		SetupModel.get().systemSettings.randomizeAmount.onChange(t -> {
			settings.randomizeAmountValue = t.value.getValue();
			settings.randomizeAmountDisplayString = t.getDecoratedValue(true, true);
			notifyChanges();
			return true;
		});

		SetupModel.get().systemSettings.transitionTime.onChange(t -> {
			settings.transitionTimeValue = t.value.getValue();
			settings.transitionTimeDisplayString = t.getDecoratedValue(true, true);
			notifyChanges();
			return true;
		});

		SetupModel.get().systemSettings.tuneReference.onChange(t -> {
			settings.tuneReferenceValue = t.value.getValue();
			settings.tuneReferenceDisplayString = t.getDecoratedValue(true, true);
			notifyChanges();
			return true;
		});

		SetupModel.get().systemSettings.syncSplit.onChange(t -> {
			settings.syncSplitsEnabled = t.equals(BooleanValues.on);
			notifyChanges();
			return true;
		});

		SetupModel.get().systemSettings.externalMidi.onChange(t -> {
			settings.externalMidiEnabled = t.equals(BooleanValues.on);
			notifyChanges();
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
			notifyChanges();
			return true;
		});
	}

	@Override
	public DeviceSettings getValue() {
		return settings;
	}
}
