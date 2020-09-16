package com.nonlinearlabs.client.presenters;

import java.util.ArrayList;
import java.util.LinkedList;
import java.util.function.Function;

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
