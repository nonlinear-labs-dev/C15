package com.nonlinearlabs.client.world.overlay.belt.presets;

import java.util.LinkedList;

import com.google.gwt.dom.client.Style.Display;
import com.google.gwt.xml.client.Node;
import com.nonlinearlabs.client.presenters.PresetManagerPresenter;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.overlay.OverlayLayout;
import com.nonlinearlabs.client.world.overlay.belt.Belt;
import com.nonlinearlabs.client.world.overlay.belt.LockSymbol;
import com.nonlinearlabs.client.world.overlay.belt.presets.html.BeltPresetsUI;

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
	private BeltPresetsUI theHtmlUI = new BeltPresetsUI();

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

	@Override
	public void calcPixRect(Position parentsReference, double currentZoom) {
		super.calcPixRect(parentsReference, currentZoom);
		theHtmlUI.syncPosition(getPixRect());
	}

	@Override
	public void movePixRect(double x, double y) {
		super.movePixRect(x, y);
		theHtmlUI.syncPosition(getPixRect());
	}

	public void update(Node settingsNode, Node editBufferNode, Node presetManagerNode) {
		store.update(settingsNode, presetManagerNode);
	}

	public BankControl getBankControl() {
		return bank.getBankControl();
	}

	public void toggleStoreSelect() {
		store.toggleStoreSelect();
	}

	public Control handleEnterKey() {
		return store.handleEnterKey();
	}

	@Override
	public void fadeIn() {
		super.fadeIn();
		theHtmlUI.getElement().getStyle().setDisplay(Display.BLOCK);
	}

	@Override
	public void fadeOut() {
		super.fadeOut();
		theHtmlUI.getElement().getStyle().setDisplay(Display.NONE);
		storeSelectOff();
	}

	public void storeSelectOff() {
		store.storeSelectOff();
	}

}