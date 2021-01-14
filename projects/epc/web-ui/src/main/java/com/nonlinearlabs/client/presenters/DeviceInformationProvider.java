package com.nonlinearlabs.client.presenters;

import com.nonlinearlabs.client.dataModel.Notifier;

public class DeviceInformationProvider extends Notifier<DeviceInformation> {
	public static DeviceInformationProvider theInstance = new DeviceInformationProvider();

	public static DeviceInformationProvider get() {
		return theInstance;
	}

	private DeviceInformation info = new DeviceInformation();

	private DeviceInformationProvider() {
		com.nonlinearlabs.client.dataModel.setup.DeviceInformation.get().freeDiscSpace.onChange(v -> {
			info.freeDiscSpace = v;
			notifyChanges();
			return true;
		});

		com.nonlinearlabs.client.dataModel.setup.DeviceInformation.get().softwareVersion.onChange(v -> {
			info.uiVersion = v;
			notifyChanges();
			return true;
		});

		com.nonlinearlabs.client.dataModel.setup.DeviceInformation.get().dateTimeDisplay.onChange(v -> {
			info.dateTime = v;
			notifyChanges();
			return true;
		});

		com.nonlinearlabs.client.dataModel.setup.DeviceInformation.get().branch.onChange(v -> {
			info.branch = v;
			notifyChanges();
			return true;
		});

		com.nonlinearlabs.client.dataModel.setup.DeviceInformation.get().head.onChange(v -> {
			info.head = v;
			notifyChanges();
			return true;
		});

		com.nonlinearlabs.client.dataModel.setup.DeviceInformation.get().commits.onChange(v -> {
			info.commits = v;
			notifyChanges();
			return true;
		});

		com.nonlinearlabs.client.dataModel.setup.DeviceInformation.get().date.onChange(v -> {
			info.commitDate = v;
			notifyChanges();
			return true;
		});

		com.nonlinearlabs.client.dataModel.setup.DeviceInformation.get().totalRam.onChange(v -> {
			info.totalRam = v;
			notifyChanges();
			return true;
		});

		com.nonlinearlabs.client.dataModel.setup.DeviceInformation.get().usedRam.onChange(v -> {
			info.usedRam = v;
			notifyChanges();
			return true;
		});
	}

	@Override
	public DeviceInformation getValue() {
		return info;
	}
}
