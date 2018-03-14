package com.nonlinearlabs.NonMaps.client.world.overlay.belt.presets;

import com.google.gwt.xml.client.Node;
import com.nonlinearlabs.NonMaps.client.world.overlay.OverlayLayout;

public class StorePresetArea extends OverlayLayout {

	private StoreOptionsMenu menu;
	private StoreModeButton m_modeButton;
	private StorePreset m_commitButton;

	protected StorePresetArea(BeltPresetLayout parent) {
		super(parent);

		m_commitButton = addChild(new StorePreset(this));
		menu = addChild(new StoreOptionsMenu(this));
		m_modeButton = addChild(new StoreModeButton(this));

	}

	public StorePreset getCommitButton() {
		return m_commitButton;
	}

	public void doLayout(double x, double y, double h) {
		double w = getDesiredWidth();
		double buttonDim = m_modeButton.getSelectedImage().getImgWidth();
		double margin = getButtonDimension() / 4;
		double contentHeight = m_modeButton.getSelectedImage().getImgHeight() + margin + menu.getPictureHeight();
		double top = (h - contentHeight) / 2;

		menu.doLayout(0, top, menu.getPictureWidth(), menu.getPictureHeight());
		m_modeButton.doLayout(0, top + menu.getPictureHeight() + margin, buttonDim, buttonDim);
		m_commitButton.doLayout(w - buttonDim, top + menu.getPictureHeight() + margin, buttonDim, buttonDim);

		super.doLayout(x, y, w, h);
	}

	public void update(Node settingsNode, Node presetManagerNode) {
		menu.update(settingsNode);
		m_commitButton.update(settingsNode, presetManagerNode);
		m_modeButton.update(settingsNode, presetManagerNode);
	}

	public double getDesiredWidth() {
		return menu.getPictureWidth();
	}
}
