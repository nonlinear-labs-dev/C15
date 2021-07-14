package com.nonlinearlabs.client;

import java.util.function.Function;

import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.VoiceGroup;
import com.nonlinearlabs.client.dataModel.setup.SetupModel;
import com.nonlinearlabs.client.world.NonLinearWorld;
import com.nonlinearlabs.client.world.maps.MapsControl;
import com.nonlinearlabs.client.world.maps.presets.PresetManager;
import com.nonlinearlabs.client.world.maps.presets.bank.Bank;
import com.nonlinearlabs.client.world.maps.presets.bank.preset.Preset;

public class LoadToPartMode implements CustomPresetSelector {

	public class LoadToPartModeData {
		public Preset m_selectedPreset;
		public Bank m_selectedBank;
		public VoiceGroup m_selectedVoiceGroup;
	};

	private PresetManager m_parent;

	private final Preset m_originPreset;

	private LoadToPartModeData m_currentData;
	private LoadToPartModeData m_otherData;

	private LoadToPartModeNotifier m_notifier;

	public LoadToPartMode(PresetManager parent) {
		m_parent = parent;

		m_notifier = new LoadToPartModeNotifier();

		m_currentData = new LoadToPartModeData();
		m_otherData = new LoadToPartModeData();

		m_originPreset = m_parent.findLoadedPreset();

		m_currentData.m_selectedPreset = m_parent.getSelectedPreset();
		m_currentData.m_selectedBank = m_parent.findBank(m_parent.getSelectedBank());
		m_currentData.m_selectedVoiceGroup = m_currentData.m_selectedPreset.isDual() ? VoiceGroup.II : VoiceGroup.I;

		m_otherData.m_selectedBank = m_currentData.m_selectedBank;
		m_otherData.m_selectedPreset = m_currentData.m_selectedPreset;
		m_otherData.m_selectedVoiceGroup = m_currentData.m_selectedPreset.isDual() ? VoiceGroup.I : VoiceGroup.II;

		EditBufferModel.get().voiceGroup.onChange((vg) -> {
			swapSelection();
			return true;
		});
	}

	private void swapSelection() {
		LoadToPartModeData tmp = m_currentData;
		m_currentData = m_otherData;
		m_otherData = tmp;
		updateUI();
	}

	public LoadToPartModeData getCurrentData() {
		return m_currentData;
	}

	public void setSelectedPreset(Preset p) {
		m_currentData.m_selectedPreset = p;
		m_currentData.m_selectedBank = p != null ? p.getParent() : null;
		updateUI();

		if (SetupModel.get().systemSettings.directLoad.getBool()) {
			NonMaps.get().getNonLinearWorld().getPresetManager().loadSelectedPresetPart();
		}
	}

	public void setSelectedBank(Bank b) {
		m_currentData.m_selectedBank = b;
		setSelectedPreset(b.getPresetList().findPreset(b.getPresetList().getSelectedPreset()));
		updateUI();
	}

	public Preset getSelectedPreset() {
		return m_currentData.m_selectedPreset;
	}

	public VoiceGroup getSelectedPart() {
		return m_currentData.m_selectedVoiceGroup;
	}

	public boolean canNext() {
		if (m_currentData.m_selectedPreset != null) {
			boolean isLastPreset = m_currentData.m_selectedPreset.getNumber() < m_currentData.m_selectedPreset
					.getParent().getPresetList().getPresetCount();

			if (m_currentData.m_selectedPreset.isDual()) {
				if (m_currentData.m_selectedVoiceGroup == VoiceGroup.I)
					return true;
				else
					return isLastPreset;
			} else {
				return isLastPreset;
			}
		}

		return false;
	}

	public boolean canPrev() {

		if (m_currentData.m_selectedPreset != null) {
			boolean isFirstPreset = m_currentData.m_selectedPreset.getNumber() > 1;

			if (m_currentData.m_selectedPreset.isDual()) {
				if (m_currentData.m_selectedVoiceGroup == VoiceGroup.II)
					return true;
				return isFirstPreset;
			} else {
				return isFirstPreset;
			}
		}

		return false;
	}

	public void selectNextPreset() {
		if (canNext()) {
			boolean selectionChanged = false;

			if (m_currentData.m_selectedPreset.isDual() && m_currentData.m_selectedVoiceGroup == VoiceGroup.I) {
				m_currentData.m_selectedVoiceGroup = VoiceGroup.II;
				selectionChanged = true;
			}

			Preset next = m_currentData.m_selectedPreset;

			if (!selectionChanged) {
				Bank b = m_currentData.m_selectedPreset.getParent();
				int idx = m_currentData.m_selectedPreset.getNumber() - 1;
				next = b.getPreset(idx + 1);
				m_currentData.m_selectedVoiceGroup = VoiceGroup.I;
			}

			setSelectedPreset(next);
		}
	}

	public void selectPreviousPreset() {
		if (canPrev()) {
			boolean selectionChanged = false;

			if (m_currentData.m_selectedPreset.isDual() && m_currentData.m_selectedVoiceGroup == VoiceGroup.II) {
				m_currentData.m_selectedVoiceGroup = VoiceGroup.I;
				selectionChanged = true;
			}

			Preset next = m_currentData.m_selectedPreset;

			if (!selectionChanged) {
				Bank b = m_currentData.m_selectedPreset.getParent();
				int idx = m_currentData.m_selectedPreset.getNumber() - 1;
				next = b.getPreset(idx - 1);

				if (next.isDual()) {
					m_currentData.m_selectedVoiceGroup = VoiceGroup.II;
				} else {
					m_currentData.m_selectedVoiceGroup = VoiceGroup.I;
				}
			}

			setSelectedPreset(next);
		}
	}

	public boolean canSelectPreviousBank() {
		if (m_currentData.m_selectedBank != null) {
			return m_currentData.m_selectedBank.getParent()
					.canSelectBankWithOrdernumberOffset(m_currentData.m_selectedBank, -1);
		}
		return false;
	}

	public boolean canSelectNextBank() {
		if (m_currentData.m_selectedBank != null) {
			return m_currentData.m_selectedBank.getParent()
					.canSelectBankWithOrdernumberOffset(m_currentData.m_selectedBank, 1);
		}
		return false;
	}

	public void selectNextBank() {
		if (canSelectNextBank()) {
			int orderNumber = m_currentData.m_selectedBank.getOrderNumber();
			selectBankWithOrderNumber(orderNumber + 1);
		}
	}

	public void selectPreviousBank() {
		if (canSelectPreviousBank()) {
			int orderNumber = m_currentData.m_selectedBank.getOrderNumber();
			selectBankWithOrderNumber(orderNumber - 1);
		}
	}

	private void selectBankWithOrderNumber(int i) {
		for (MapsControl c : m_currentData.m_selectedBank.getParent().getChildren()) {
			if (c instanceof Bank) {
				Bank b = (Bank) c;
				if (b.getOrderNumber() == i) {
					setSelectedBank(b);
					return;
				}
			}
		}
	}

	public Preset getOriginalPreset() {
		return m_originPreset;
	}

	public Bank getSelectedBank() {
		return m_currentData.m_selectedBank;
	}

	public void onChange(Function<Void, Boolean> cb) {
		m_notifier.onChange(cb);
	}

	public void updateUI() {
		NonMaps.get().getNonLinearWorld().invalidate(NonLinearWorld.INVALIDATION_FLAG_UI_CHANGED);
		NonMaps.get().getNonLinearWorld().getViewport().getOverlay().getBelt().getPresetLayout().getBankControl()
				.update();

		m_notifier.notifyChanges();
	}

	public boolean isOriginalPreset(Preset mapsPreset) {
		return mapsPreset == m_originPreset;
	}

}
