package com.nonlinearlabs.client.world.overlay.belt.presets;

import com.google.gwt.xml.client.Node;
import com.nonlinearlabs.client.Millimeter;
import com.nonlinearlabs.client.presenters.PresetManagerPresenterProvider;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.overlay.OverlayLayout;

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
		double margin = Millimeter.toPixels(2.5);
		double contentHeight = m_modeButton.getSelectedImage().getImgHeight() + margin + menu.getPictureHeight();
		double top = (h - contentHeight) / 2;

		menu.doLayout(margin, top, menu.getPictureWidth(), menu.getPictureHeight());
		m_modeButton.doLayout(margin, top + menu.getPictureHeight() + margin, buttonDim, buttonDim);
		m_commitButton.doLayout(margin + w - buttonDim, top + menu.getPictureHeight() + margin, buttonDim, buttonDim);

		super.doLayout(x, y, w + 2 * margin, h);
	}

	public void update(Node settingsNode, Node presetManagerNode) {
		menu.update(settingsNode);
		m_commitButton.update(settingsNode, presetManagerNode);
		m_modeButton.update(settingsNode, presetManagerNode);
	}

	public double getDesiredWidth() {
		return menu.getPictureWidth();
	}

	public void toggleStoreSelect() {
		m_modeButton.toggle();
	}

	public Control handleEnterKey() {
		if (PresetManagerPresenterProvider.get().getValue().inStoreSelectMode) {
			return m_commitButton.handleEnterKey();
		}
		return null;
	}

	public void storeSelectOff() {
		m_modeButton.storeSelectOff();

	}
}
