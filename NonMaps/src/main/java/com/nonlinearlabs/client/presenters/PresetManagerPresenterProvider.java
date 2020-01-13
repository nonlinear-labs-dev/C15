package com.nonlinearlabs.client.presenters;

import java.util.LinkedList;
import java.util.Map;
import java.util.function.Function;

import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.SoundType;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.VoiceGroup;
import com.nonlinearlabs.client.dataModel.presetManager.Bank;
import com.nonlinearlabs.client.dataModel.setup.SetupModel;
import com.nonlinearlabs.client.dataModel.setup.SetupModel.LoadMode;

public class PresetManagerPresenterProvider {
	public static PresetManagerPresenterProvider theInstance = new PresetManagerPresenterProvider();

	public static PresetManagerPresenterProvider get() {
		return theInstance;
	}

	private LinkedList<Function<PresetManagerPresenter, Boolean>> clients;
	private PresetManagerPresenter pm = null;

	public PresetManagerPresenterProvider() {
		pm = new PresetManagerPresenter();
		clients = new LinkedList<Function<PresetManagerPresenter, Boolean>>();
		com.nonlinearlabs.client.dataModel.presetManager.PresetManagerModel.get().getBanks()
				.onChange(b -> onBanksChanged(b));

		SetupModel.get().systemSettings.loadMode.onChange(m -> {
			updateLoadMode();
			return true;
		});

		EditBufferModel.get().soundType.onChange(m -> {
			updateLoadMode();
			return true;
		});

		EditBufferModel.get().voiceGroup.onChange(m -> {
			updateLoadMode();
			return true;
		});
	}

	private void updateLoadMode() {
		LoadMode l = SetupModel.get().systemSettings.loadMode.getValue();
		SoundType s = EditBufferModel.get().soundType.getValue();
		VoiceGroup v = EditBufferModel.get().voiceGroup.getValue();

		pm.loadToPartEnabled = s != SoundType.Single;

		if(pm.loadToPartEnabled)
			pm.loadModeMenuEntries = new String[] { "Load to " + v.toString(), "Select Only", "Direct Load" };
		else
			pm.loadModeMenuEntries = new String[] { "", "Select Only", "Direct Load"};


		pm.loadModeButtonState = l == LoadMode.DirectLoad;
		pm.directLoadActive = l == LoadMode.DirectLoad;
		pm.loadToPartActive = l == LoadMode.LoadToPart;

		notifyClients();
	}

	private Boolean onBanksChanged(Map<String, Bank> dmBanks) {
		boolean removed = pm.banks.removeIf(b -> !dmBanks.containsKey(b.uuid));

		int numBanksBefore = pm.banks.size();
		dmBanks.forEach((key, bank) -> {
			try {
				if (!pm.banks.stream().anyMatch((b) -> b.uuid.equals(key))) {
					pm.banks.add(new PresetManagerPresenter.Bank(bank));
				}
			} catch (Exception e) {
			}
		});

		int numBanksAfter = pm.banks.size();
		boolean inserted = numBanksAfter != numBanksBefore;
		boolean changed = inserted || removed;

		if (changed)
			notifyClients();

		return true;
	}

	protected void notifyClients() {
		clients.removeIf(listener -> !listener.apply(pm));
	}

	public void register(Function<PresetManagerPresenter, Boolean> cb) {
		clients.add(cb);
		cb.apply(pm);
	}

	public PresetManagerPresenter getPresenter() {
		return pm;
	}
}
