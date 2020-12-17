package com.nonlinearlabs.client.presenters;

import java.util.ArrayList;
import java.util.LinkedList;
import java.util.function.Function;

import com.google.gwt.i18n.client.NumberFormat;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel;
import com.nonlinearlabs.client.dataModel.presetManager.Banks;
import com.nonlinearlabs.client.dataModel.presetManager.LoadToPartMode;
import com.nonlinearlabs.client.dataModel.presetManager.PresetManagerModel;
import com.nonlinearlabs.client.dataModel.presetManager.PresetManagerModel.DragDataType;
import com.nonlinearlabs.client.dataModel.presetManager.Presets;
import com.nonlinearlabs.client.dataModel.presetManager.StoreSelectMode;

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

		Presets.get().onChange(v -> {
			var hasPresets = !v.isEmpty();
			if (hasPresets != pm.hasPresets) {
				pm.hasPresets = hasPresets;
				notifyClients();
			}
			return true;
		});

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

				pm.selectedPreset = selPreset;
				updateSelectionOpportunities();
				notifyClients();
				return true;
			});

			notifyClients();
			return true;
		});

		model.customPresetSelection.onChange(v -> {
			var loadToPart = v instanceof LoadToPartMode;
			if (pm.inLoadToPartMode != loadToPart) {
				pm.inLoadToPartMode = loadToPart;
				updateSelectionOpportunities();
				notifyClients();
			}

			var storeSelect = v instanceof StoreSelectMode;
			if (pm.inStoreSelectMode != storeSelect) {
				pm.inStoreSelectMode = storeSelect;
				updateSelectionOpportunities();
				notifyClients();
			}

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
				updateSelectionOpportunities();
				notifyClients();
			}

			return true;
		});

		model.fileVersion.onChange(v -> {
			pm.fileVersion = v;
			notifyClients();
			return true;
		});

		EditBufferPresenterProvider.get().onChange(p -> updateLoadedPresetNumber());
	}

	public boolean updateLoadedPresetNumber() {
		String loadedPresetUUID = EditBufferModel.get().loadedPreset.getValue();
		var preset = Presets.get().find(loadedPresetUUID);
		var bank = Banks.get().find(preset != null ? preset.bankUuid.getValue() : "");
		var modFlag = EditBufferPresenterProvider.getPresenter().isAnyParameterChanged ? " *" : "";

		if (EditBufferModel.get().loadedPreset.getValue().equals("Init")) {
			pm.loadedPresetNumber = "Init" + modFlag;
		} else if (EditBufferModel.get().loadedPreset.getValue().equals("Converted")) {
			pm.loadedPresetNumber = "Converted" + modFlag;
		} else if (bank == null && preset == null) {
			pm.loadedPresetNumber = "";
		} else if (bank != null && preset != null) {
			pm.loadedPresetNumber = bank.orderNumber.getValue() + "-"
					+ NumberFormat.getFormat("000").format(preset.number.getValue()) + modFlag;
		} else {
			var loadedPresetUuid = EditBufferModel.get().loadedPreset.getValue();
			var loadedPreset = Presets.get().find(loadedPresetUuid);
			var bankUuid = loadedPreset != null ? loadedPreset.bankUuid.getValue() : "";

			if (bank != null && loadedPreset != null) {
				var bankOrderNumber = BankPresenterProviders.get().getPresenter(bankUuid).orderNumber;
				var presetOrderNumber = PresetPresenterProviders.get().getPresenter(loadedPresetUuid).paddedNumber;
				pm.loadedPresetNumber = bankOrderNumber + "-" + presetOrderNumber + modFlag;
			}
		}

		return true;
	}

	private void updateSelectionOpportunities() {
		var model = PresetManagerModel.get();
		var idx = model.banks.getValue().indexOf(model.selectedBank.getValue());
		var canPrevBank = idx > 0;
		var canNextBank = idx < model.banks.getValue().size() - 1;

		if (pm.canSelectNextBank != canNextBank) {
			pm.canSelectNextBank = canNextBank;
			notifyClients();
		}

		if (pm.canSelectPrevBank != canPrevBank) {
			pm.canSelectPrevBank = canPrevBank;
			notifyClients();
		}

	}

	private Boolean onBanksChanged(ArrayList<String> dmBanks) {
		pm.banks = dmBanks;
		updateSelectionOpportunities();
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
