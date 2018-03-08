package com.nonlinearlabs.NonMaps.client.world.maps.presets.bank;

import com.nonlinearlabs.NonMaps.client.NonMaps;
import com.nonlinearlabs.NonMaps.client.world.Control;
import com.nonlinearlabs.NonMaps.client.world.NonLinearWorld;
import com.nonlinearlabs.NonMaps.client.world.maps.LayoutResizingVertical;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.Parameter.Initiator;
import com.nonlinearlabs.NonMaps.client.world.maps.presets.PresetManager;
import com.nonlinearlabs.NonMaps.client.world.maps.presets.bank.preset.Preset;
import com.nonlinearlabs.NonMaps.client.world.overlay.belt.presets.PresetList.ScrollRequest;

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

	public void selectPrev(Initiator initiator) {
		Preset prev = getPrev(selectedPreset);

		if (prev != null)
			selectPreset(prev.getUUID(), initiator);
	}

	public void selectNext(Initiator initiator) {
		Preset next = getNext(selectedPreset);

		if (next != null)
			selectPreset(next.getUUID(), initiator);
	}

	public String getSelectedPreset() {
		return selectedPreset;
	}

	public void selectPreset(String uuid, Initiator initiator) {
		if (selectedPreset.equals(uuid))
			return;

		NonLinearWorld world = NonMaps.theMaps.getNonLinearWorld();
		PresetManager pm = getParent().getParent();
		Preset p = findPreset(uuid);

		selectedPreset = uuid;

		if (initiator == Initiator.EXPLICIT_USER_ACTION) {
			if (world.isShiftDown()) {
				if (pm.hasMultiplePresetSelection())
					pm.getMultiSelection().toggle(p);
				else
					pm.startMultiSelection(p);
				return;
			}

			getNonMaps().getServerProxy().selectPreset(uuid);
		}

		requestLayout();

		if (getParent().isSelected()) {
			world.getViewport().getOverlay().getBelt().getPresetLayout().getBankControl().getPresetList()
					.scheduleAutoScroll(ScrollRequest.Smooth);
		}

		pm.onPresetSelectionChanged(findPreset(selectedPreset));
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
