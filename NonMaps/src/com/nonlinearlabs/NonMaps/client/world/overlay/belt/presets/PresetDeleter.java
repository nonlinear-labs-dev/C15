package com.nonlinearlabs.NonMaps.client.world.overlay.belt.presets;

import com.google.gwt.user.client.ui.Button;
import com.google.gwt.user.client.ui.HTMLPanel;
import com.google.gwt.user.client.ui.Label;
import com.nonlinearlabs.NonMaps.client.NonMaps;
import com.nonlinearlabs.NonMaps.client.world.Control;
import com.nonlinearlabs.NonMaps.client.world.maps.presets.bank.preset.Preset;
import com.nonlinearlabs.NonMaps.client.world.overlay.GWTDialog;

public class PresetDeleter extends GWTDialog {

	public static PresetDeleter instance = null;
	private Button m_no, m_yes, m_cancel;
	private Preset thePreset;
	private String presetCSV = "";

	protected PresetDeleter(Preset p) {
		this();
		thePreset = p;
	}

	protected PresetDeleter(String s) {
		this();
		presetCSV = s;
	}

	protected PresetDeleter() {

		setText("Delete Presets");
		HTMLPanel x = new HTMLPanel("");
		x.add(new Label("Do you also want to delete the Bank?"));
		x.add(m_no = new Button("No"));
		m_no.setFocus(true);
		x.add(m_yes = new Button("Yes"));
		x.add(m_cancel = new Button("Cancel"));
		add(x);

		m_no.addClickHandler(event -> delete(false));
		m_yes.addClickHandler(event -> delete(true));
		m_cancel.addClickHandler(event -> cancel());
		setPopupPosition(getLastPopupPosLeft() - 150, getLastPopupPosTop() - 100);
		show();
	}

	public static void open(String csv) {
		instance = new PresetDeleter(csv);
		instance.show();
	}

	public static void open(Preset p) {
		instance = new PresetDeleter(p);
		instance.show();
	}

	private void delete(boolean withBank) {
		if (!presetCSV.isEmpty()) {
			NonMaps.get().getServerProxy().deletePresets(presetCSV, withBank);
		} else {
			NonMaps.get().getServerProxy().deletePreset(thePreset, withBank);
		}
		commit();
	}

	public boolean isPresetInSelection(Preset p) {
		if (thePreset == p)
			return true;

		return presetCSV.contains(p.getUUID());
	}

	private void cancel() {
		commit();
	}

	@Override
	protected void setLastPopupPos(int popupLeft, int popupTop) {
	}

	@Override
	protected int getLastPopupPosTop() {
		return NonMaps.get().getCanvas().getCanvasElement().getHeight() / 2;
	}

	@Override
	protected int getLastPopupPosLeft() {
		return NonMaps.get().getCanvas().getCanvasElement().getWidth() / 2;
	}

	@Override
	protected void commit() {
		thePreset = null;
		presetCSV = "";
		NonMaps.get().getNonLinearWorld().invalidate(Control.INVALIDATION_FLAG_UI_CHANGED);
		hide();
	}
}
