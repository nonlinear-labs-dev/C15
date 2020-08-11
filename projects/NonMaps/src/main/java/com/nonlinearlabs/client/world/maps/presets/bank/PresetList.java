package com.nonlinearlabs.client.world.maps.presets.bank;

import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.maps.LayoutResizingVertical;
import com.nonlinearlabs.client.world.maps.presets.bank.preset.Preset;
import com.nonlinearlabs.client.world.overlay.belt.presets.PresetList.ScrollRequest;

public class PresetList extends LayoutResizingVertical {

	private String selectedPreset = "";

	public PresetList(Bank parent) {
		super(parent);
	}

	@Override
	public Bank getParent() {
		return (Bank) super.getParent();
	}

	public Preset findLoadedPreset() {
		for (Control c : getChildren()) {
			if (c instanceof Preset) {
				Preset p = (Preset) c;
				if (p.isLoaded())
					return p;
			}
		}
		return null;
	}

	public boolean isLast(Preset p) {
		boolean found = false;

		for (Control c : getChildren()) {
			if (c == p) {
				found = true;
			} else if (c instanceof Preset && found)
				return false;
		}

		return found;
	}

	public boolean isFirst(Preset p) {
		for (Control c : getChildren()) {
			if (c instanceof Preset) {
				return c == p;
			}
		}

		return false;
	}

	public boolean doesBankContainLoadedPreset() {
		for (Control c : getChildren()) {
			if (c instanceof Preset) {
				Preset p = (Preset) c;
				if (p.isLoaded())
					return true;
			}
		}
		return false;
	}

	public Preset getPrev(String uuid) {
		Preset last = null;

		for (Control c : getChildren()) {
			if (c instanceof Preset) {
				Preset p = (Preset) c;
				if (p.getUUID().equals(uuid))
					return last;

				last = p;
			}
		}
		return null;
	}

	public Preset getNext(String uuid) {
		boolean found = false;

		for (Control c : getChildren()) {
			if (c instanceof Preset) {
				Preset p = (Preset) c;

				if (found)
					return p;

				if (p.getUUID().equals(uuid))
					found = true;
			}
		}
		return null;
	}

	public void selectPrev() {
		Preset prev = getPrev(selectedPreset);

		if (prev != null)
			selectPreset(prev.getUUID(), true);
	}

	public void selectNext() {
		Preset next = getNext(selectedPreset);

		if (next != null)
			selectPreset(next.getUUID(), true);
	}

	public String getSelectedPreset() {
		return selectedPreset;
	}

	public void selectPreset(String uuid, boolean sendToServer) {
		selectedPreset = uuid;

		if(sendToServer)
			getNonMaps().getServerProxy().selectPreset(uuid);

		requestLayout();

		if (getParent().isSelected()) {
			NonMaps.theMaps.getNonLinearWorld().getViewport().getOverlay().getBelt().getPresetLayout().getBankControl()
					.getPresetList().scheduleAutoScroll(ScrollRequest.Smooth);
		}

		getParent().getParent().onPresetSelectionChanged(findPreset(selectedPreset));
	}

	public boolean hasSelectedPreset() {
		return findPreset(selectedPreset) != null;
	}

	public boolean canNext() {
		Preset p = findPreset(selectedPreset);
		if (p != null)
			return !isLast(p);
		return false;
	}

	public boolean canPrev() {
		Preset p = findPreset(selectedPreset);
		if (p != null)
			return !isFirst(p);
		return false;
	}

	public short getPresetCount() {
		short numberOfPresets = 0;
		for (Control c : getChildren()) {
			if (c instanceof Preset) {
				numberOfPresets++;
			}
		}
		return numberOfPresets;
	}

	public Preset findPreset(String uuid) {
		for (Control c : getChildren()) {
			if (c instanceof Preset) {
				Preset p = (Preset) c;

				if (p.getUUID().equals(uuid))
					return p;
			}
		}
		return null;
	}

}
