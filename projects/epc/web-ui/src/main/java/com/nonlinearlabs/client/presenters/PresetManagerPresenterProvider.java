package com.nonlinearlabs.client.presenters;

import java.util.ArrayList;
import java.util.LinkedList;
import java.util.function.Function;

import com.nonlinearlabs.client.dataModel.presetManager.PresetManagerModel;

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
		PresetManagerModel.get().banks.onChange(b -> onBanksChanged(b));

		PresetManagerModel.get().positions.onChange(p -> {
			pm.bankPositions = p;
			notifyClients();
			return true;
		});
	}

	private Boolean onBanksChanged(ArrayList<String> dmBanks) {
		pm.banks = dmBanks;
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
