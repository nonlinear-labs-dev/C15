package com.nonlinearlabs.client.world.maps.presets.bank;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;

import com.google.gwt.core.client.GWT;
import com.google.gwt.xml.client.Node;
import com.google.gwt.xml.client.NodeList;
import com.nonlinearlabs.client.ServerProxy;
import com.nonlinearlabs.client.world.maps.MapsControl;
import com.nonlinearlabs.client.world.maps.NonPosition;
import com.nonlinearlabs.client.world.maps.presets.bank.preset.Preset;

public class Updater {

	private Bank bank;

	public Updater(Bank bank) {
		this.bank = bank;
	}

	public void update(Node bank, boolean force) {
		if (force || ServerProxy.didChange(bank)) {
			String selectedPreset = bank.getAttributes().getNamedItem("selected-preset").getNodeValue();
			GWT.log("selected Preset:" + selectedPreset);
			
			updateBankName(bank);
			updateBankPosition(bank);
			updateOrderNumber(bank);
			this.bank.updateAttributes(bank);
			this.bank.getPresetList().selectPreset(selectedPreset, false);
			updatePresets(bank, force);
			updateAttachment(bank);
			updateDateOfLastChange(bank);
			handleEmptyBank();
		}
	}

	private void updateAttachment(Node bank) {
		String masterUUID = ServerProxy.getChildText(bank, "attached-to");
		String direction = ServerProxy.getChildText(bank, "attached-direction");

		this.bank.setAttatchedTo(direction);
		this.bank.setMasterUUID(masterUUID);
	}

	private void updateDateOfLastChange(Node bank) {
		String d = ServerProxy.getChildText(bank, "date-of-last-change");
		String state = ServerProxy.getChildText(bank, "state");
		this.bank.setDateOfLastChange(d);
		this.bank.setImportExportState(state);
	}

	private void handleEmptyBank() {
		bank.showEmptyLabel(isBankEmpty());
	}

	private boolean isBankEmpty() {
		return bank.getPresetList().getChildren().isEmpty();
	}

	private void updatePresets(Node bank, boolean force) {
		NodeList presets = bank.getChildNodes();
		ArrayList<MapsControl> currentChildren = collectCurrentPresets();

		int presetOrderNumber = 1;

		for (int i = 0; i < presets.getLength(); i++) {
			Node child = presets.item(i);
			if (child != null && child.getNodeName().equals("preset")) {
				Preset presetUI = updatePreset(presetOrderNumber++, presets.item(i));
				currentChildren.remove(presetUI);
			}
		}

		removeObsoletePresets(currentChildren);
		sortByOrderNumber();
	}

	private void sortByOrderNumber() {
		Collections.sort(this.bank.getPresetList().getChildren(), new Comparator<MapsControl>() {

			@Override
			public int compare(MapsControl a, MapsControl b) {
				Preset pa = (Preset) a;
				Preset pb = (Preset) b;
				return pa.getNumber() - pb.getNumber();
			}
		});
	}

	@SuppressWarnings("unchecked")
	private ArrayList<MapsControl> collectCurrentPresets() {
		return (ArrayList<MapsControl>) this.bank.getPresetList().getChildren().clone();
	}

	private void updateBankName(Node bank) {
		this.bank.header.setBankName(bank.getAttributes().getNamedItem("name").getNodeValue());
	}

	private void updateOrderNumber(Node bank) {
		String v = bank.getAttributes().getNamedItem("order-number").getNodeValue();
		try {
			int i = Integer.parseInt(v);
			this.bank.setOrderNumber(i);
		} catch (Exception e) {
		}
	}

	private void updateBankPosition(Node bank) {
		boolean updateServersBankPos = false;

		String x = bank.getAttributes().getNamedItem("x").getNodeValue();
		String y = bank.getAttributes().getNamedItem("y").getNodeValue();

		NonPosition bankPos = this.bank.getNonMaps().getNonLinearWorld().getViewport().getNonPosition()
				.getCenterPoint();

		try {
			bankPos = new NonPosition(Double.parseDouble(x), Double.parseDouble(y));
		} catch (Exception ex) {
			updateServersBankPos = true;
		}

		if (this.bank.getNonPosition().moveTo(bankPos))
			this.bank.requestLayout();

		if (updateServersBankPos)
			this.bank.getNonMaps().getServerProxy().onBankPositionChanged(this.bank);
	}

	private void removeObsoletePresets(ArrayList<MapsControl> currentChildren) {
		boolean needsLayout = false;

		for (MapsControl control : currentChildren) {
			this.bank.getPresetList().removeChild(control);
			needsLayout = true;
		}

		if (needsLayout)
			this.bank.requestLayout();
	}

	private Preset updatePreset(int presetOrderNumber, Node presetNode) {
		String uuid = presetNode.getAttributes().getNamedItem("uuid").getNodeValue();
		Preset presetUI = bank.getPresetList().findPreset(uuid);

		if (presetUI != null) {
			presetUI.update(presetOrderNumber, presetNode);
			return presetUI;
		}

		presetUI = new Preset(this.bank);
		this.bank.getPresetList().addChild(presetUI);
		presetUI.update(presetOrderNumber, presetNode);
		this.bank.requestLayout();
		return presetUI;
	}
}
