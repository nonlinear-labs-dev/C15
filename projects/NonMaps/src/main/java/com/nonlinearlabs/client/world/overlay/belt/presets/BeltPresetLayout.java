package com.nonlinearlabs.client.world.overlay.belt.presets;

import java.util.LinkedList;

import com.google.gwt.xml.client.Node;
import com.nonlinearlabs.client.presenters.PresetManagerPresenter;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.overlay.OverlayLayout;
import com.nonlinearlabs.client.world.overlay.belt.Belt;
import com.nonlinearlabs.client.world.overlay.belt.LockSymbol;

public class BeltPresetLayout extends OverlayLayout {
	MenuArea menu;
	StorePresetArea store;
	BankControlArea bank;
	LoadButtonArea load;
	CurrentPresetNumber currentPreset;
	LoadModeMenu autoLoad;
	LockSymbol lock;
	PresetManagerPresenter presenter;
	boolean showLoadModeMenu = false;

	private LinkedList<PresetBeltLayouter> layouters = new LinkedList<PresetBeltLayouter>();

	public BeltPresetLayout(Belt parent) {
		super(parent);

		addChild(menu = new MenuArea(this));
		addChild(store = new StorePresetArea(this));
		addChild(bank = new BankControlArea(this));
		addChild(load = new LoadButtonArea(this));
		addChild(currentPreset = new CurrentPresetNumber(this));
		addChild(autoLoad = new LoadModeMenu(this));
		addChild(lock = new LockSymbol(this));

		layouters.add(new PresetBeltLayouterXXL(this));
		layouters.add(new PresetBeltLayouterXL(this));
		layouters.add(new PresetBeltLayouterL(this));
		layouters.add(new PresetBeltLayouterM(this));
		layouters.add(new PresetBeltLayouterS(this));
		layouters.add(new PresetBeltLayouterXS(this));
		layouters.add(new PresetBeltLayouterXXS(this));
	}

	@Override
	public Belt getParent() {
		return (Belt) super.getParent();
	}

	@Override
	public void doLayout(double x, double y, double w, double h) {
		super.doLayout(x, y, w, h);

		for (PresetBeltLayouter layouter : layouters)
			if (layouter.doLayout(w, h))
				return;
	}

	public void update(Node settingsNode, Node editBufferNode, Node presetManagerNode) {
		bank.update(presetManagerNode);
		store.update(settingsNode, presetManagerNode);
	}

	public BankControl getBankControl() {
		return bank.getBankControl();
	}

	public void renameCurrentPreset() {
		bank.renameCurrentPreset();
	}

	public void toggleStoreSelect() {
		store.toggleStoreSelect();
	}

	public Control handleEnterKey() {
		return store.handleEnterKey();
	}

	@Override
	public void fadeOut() {
		super.fadeOut();
		storeSelectOff();
	}

	public void storeSelectOff() {
		store.storeSelectOff();
	}

}