package com.nonlinearlabs.client.presenters;

import java.util.ArrayList;
import java.util.LinkedList;
import java.util.function.Function;

import com.nonlinearlabs.client.dataModel.presetManager.Banks;
import com.nonlinearlabs.client.dataModel.presetManager.PresetManagerModel;
import com.nonlinearlabs.client.dataModel.presetManager.PresetManagerModel.DragDataType;

public class PresetManagerPresenterProvider {
	public static PresetManagerPresenterProvider theInstance = new PresetManagerPresenterProvider();

	public static PresetManagerPresenterProvider get() {
		return theInstance;
	}

	private LinkedList<Function<PresetManagerPresenter, Boolean>> clients;
	private PresetManagerPresenter pm = null;

	public PresetManagerPresenterProvider() {
		var model = PresetManagerModel.get();

		pm = new PresetManagerPresenter();

		clients = new LinkedList<Function<PresetManagerPresenter, Boolean>>();

		model.banks.onChange(b -> onBanksChanged(b));
		model.positions.onChange(p -> {
			pm.bankPositions = p;
			notifyClients();
			return true;
		});

		model.dnd.onChange(p -> {
			if (p != null)
				pm.dndType = p.type;
			else
				pm.dndType = DragDataType.None;
			notifyClients();
			return true;
		});

		model.selectedBank.onChange(v -> {
			pm.selectedBank = v;

			Banks.get().findOrPut(v).selectedPreset.onChange(selPreset -> {
				if (v != model.selectedBank.getValue())
					return false;

				pm.selectedPresetOfSelectedBank = selPreset;
				return true;
			});

			notifyClients();
			return true;
		});

		model.selectedPresets.onChange(v -> {
			var multiSelection = v != null;
			var numSelectedPresets = (v != null) ? v.size() : 0;

			if (pm.multiSelection != multiSelection || numSelectedPresets != pm.numSelectedPresetsInMultiSelection
					|| pm.currentMultiSelection != v) {
				pm.multiSelection = multiSelection;
				pm.numSelectedPresetsInMultiSelection = numSelectedPresets;
				pm.currentMultiSelection = v;
				notifyClients();
			}

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
