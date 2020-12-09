package com.nonlinearlabs.client.world.maps.presets;

import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;
import java.util.Set;
import java.util.function.Function;

import com.google.gwt.event.dom.client.KeyDownEvent;
import com.google.gwt.user.client.Window;
import com.google.gwt.xml.client.Node;
import com.google.gwt.xml.client.NodeList;
import com.nonlinearlabs.client.CustomPresetSelector;
import com.nonlinearlabs.client.LoadToPartMode;
import com.nonlinearlabs.client.LoadToPartModeNotifier;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.Renameable;
import com.nonlinearlabs.client.ServerProxy;
import com.nonlinearlabs.client.StoreSelectMode;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.SoundType;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.VoiceGroup;
import com.nonlinearlabs.client.dataModel.presetManager.PresetSearch;
import com.nonlinearlabs.client.dataModel.setup.SetupModel;
import com.nonlinearlabs.client.dataModel.setup.SetupModel.BooleanValues;
import com.nonlinearlabs.client.presenters.LocalSettingsProvider;
import com.nonlinearlabs.client.presenters.PresetManagerPresenterProvider;
import com.nonlinearlabs.client.useCases.EditBufferUseCases;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.IPreset;
import com.nonlinearlabs.client.world.NonLinearWorld;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.RenameDialog;
import com.nonlinearlabs.client.world.maps.MapsControl;
import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.NonDimension;
import com.nonlinearlabs.client.world.maps.NonPosition;
import com.nonlinearlabs.client.world.maps.NonRect;
import com.nonlinearlabs.client.world.maps.presets.bank.Bank;
import com.nonlinearlabs.client.world.maps.presets.bank.Tape;
import com.nonlinearlabs.client.world.maps.presets.bank.Updater;
import com.nonlinearlabs.client.world.maps.presets.bank.preset.Preset;
import com.nonlinearlabs.client.world.overlay.BankInfoDialog;
import com.nonlinearlabs.client.world.overlay.DragProxy;
import com.nonlinearlabs.client.world.overlay.ParameterInfoDialog;
import com.nonlinearlabs.client.world.overlay.PresetInfoDialog;
import com.nonlinearlabs.client.world.overlay.belt.EditBufferDraggingButton;
import com.nonlinearlabs.client.world.overlay.belt.presets.PresetContextMenu;
import com.nonlinearlabs.client.world.overlay.html.presetSearch.PresetSearchDialog;
import com.nonlinearlabs.client.world.pointer.PointerState;

public class PresetManager extends MapsLayout {

	private String selectedBank;

	private int currentFileVersion = 0;
	private MultiplePresetSelection multiSelection;
	private MoveAllBanksLayer moveAllBanks;
	private MoveSomeBanksLayer moveSomeBanks;

	private CustomPresetSelector customPresetSelector = null;

	private LoadToPartModeNotifier loadToPartNotifier = null;
	private Tape attachingTapes[] = new Tape[2];

	private boolean scrollToSelectedPresetScheduled = false;

	private static NonRect oldView = null;

	public List<Bank> getBanks() {
		List<Bank> ret = new ArrayList<>();
		for (Control c : getChildren()) {
			if (c instanceof Bank) {
				ret.add((Bank) c);
			}
		}
		return ret;
	}

	public void saveView() {
		if (NonMaps.theMaps.getNonLinearWorld() != null && NonMaps.theMaps.getNonLinearWorld().getViewport() != null)
			oldView = NonMaps.theMaps.getNonLinearWorld().getViewport().getNonPosition().copy();
	}

	public void resetView() {
		if (NonMaps.theMaps.getNonLinearWorld() != null && oldView != null)
			NonMaps.theMaps.getNonLinearWorld().animateViewport(oldView, true);

		oldView = null;
	}

	public void resetStoredViewportPosition() {
		oldView = null;
	}

	public boolean hasStoredViewportPosition() {
		return oldView != null;
	}

	public PresetManager(NonLinearWorld parent) {
		super(parent);

		PresetSearch.get().searchActive.onChange(b -> {
			if (b == BooleanValues.on) {
				saveView();
			} else {
				resetView();
			}
			return true;
		});

		PresetSearch.get().currentFilterMatch.onChange(b -> {
			if (PresetSearch.get().results.getValue().isEmpty()) {
				resetView();
			} else {
				zoomToAllFilterMatches();
			}
			return true;
		});

		PresetSearch.get().zoomToMatches.onChange(b -> {
			if (b == BooleanValues.on) {
				saveView();
				zoomToAllFilterMatches();
			} else {
				resetView();
			}
			return true;
		});

		PresetSearch.get().results.onChange(b -> {
			zoomToAllFilterMatches();
			return true;
		});

		EditBufferModel.get().soundType.onChange(type -> {
			if (type == SoundType.Single) {
				endLoadToPartMode();
				return true;
			}
			return true;
		});

		loadToPartNotifier = new LoadToPartModeNotifier();
	}

	public boolean hasCustomPresetSelection() {
		return customPresetSelector != null;
	}

	public void startStoreSelectMode() {
		if (isInLoadToPartMode())
			customPresetSelector = null;

		if (customPresetSelector == null) {
			if (isEmpty() == false) {
				customPresetSelector = new StoreSelectMode(this);
				customPresetSelector.updateUI();
			}
		}
	}

	public void endStoreSelectMode() {
		if (customPresetSelector != null) {
			CustomPresetSelector tmp = customPresetSelector;
			customPresetSelector = null;
			tmp.updateUI();
		}
	}

	public void onLoadToPartModeToggled(Function<Void, Boolean> cb) {
		loadToPartNotifier.onChange(cb);
	}

	public void startLoadToPartMode() {
		if (isInStoreSelectMode())
			customPresetSelector = null;

		if (!isEmpty()) {
			customPresetSelector = new LoadToPartMode(this);
			loadToPartNotifier.notifyChanges();
			customPresetSelector.updateUI();
		}
	}

	public void endLoadToPartMode() {
		if (customPresetSelector != null) {
			CustomPresetSelector tmp = customPresetSelector;
			customPresetSelector = null;
			loadToPartNotifier.notifyChanges();
			tmp.updateUI();
		}
	}

	public void setAllBanksCollapseState(boolean min) {
		for (Bank b : getBanks()) {
			b.setCollapsed(min);
		}
	}

	public boolean isAnyBankCollapsed() {
		for (Bank b : getBanks()) {
			if (b.isCollapsed())
				return true;
		}
		return false;
	}

	public boolean areAllBanksCollapsed() {
		for (Bank b : getBanks()) {
			if (!b.isCollapsed())
				return false;
		}
		return true;
	}

	public MoveSomeBanksLayer getMoveSomeBanks() {
		return moveSomeBanks;
	}

	@Override
	public NonLinearWorld getParent() {
		return (NonLinearWorld) super.getParent();
	}

	@Override
	public double getLevelOfDetailForFullVisibility() {
		return 1;
	}

	private NonRect getAllBanksOutline() {
		NonRect fence = new NonRect(Double.MAX_VALUE / 2, Double.MAX_VALUE / 2, -Double.MAX_VALUE, -Double.MAX_VALUE);

		for (MapsControl c : getChildren()) {
			if (c instanceof Bank) {
				NonRect banksRect = c.getNonPosition();
				double right = fence.getRight();
				double bottom = fence.getBottom();
				fence.setLeft(Math.min(banksRect.getLeft(), fence.getLeft()));
				fence.setTop(Math.min(banksRect.getTop(), fence.getTop()));
				fence.setBottom(Math.max(banksRect.getBottom(), bottom));
				fence.setRight(Math.max(banksRect.getRight(), right));
			}
		}
		return fence;
	}

	@Override
	public void calcPixRect(Position parentsReference, double currentZoom) {
		if (super.calcPixRectWithoutMargins(parentsReference, currentZoom))
			children.calcPixRect(this.getPixRect().getCenterPoint(), currentZoom);

		if (scrollToSelectedPresetScheduled) {
			scrollToSelectedPresetScheduled = false;
			scrollToSelectedPreset();
		}
	}

	public void update(Node presetManagerNode) {

		if (ServerProxy.didChange(presetManagerNode)) {
			readPlaygroundFileVersion(presetManagerNode);

			Preset oldPresetSelection = getSelectedPreset();
			NodeList children = presetManagerNode.getChildNodes();

			for (int i = 0; i < children.getLength(); i++) {
				Node child = children.item(i);

				if (child.getNodeName().equals("banks"))
					if (ServerProxy.didChange(child))
						updateBanks(child);
			}

			Preset newPresetSelection = getSelectedPreset();

			if (oldPresetSelection != newPresetSelection) {
				onPresetSelectionChanged(newPresetSelection);
			}

			RenameDialog.onPresetManagerUpdate(this);
		}
	}

	private void readPlaygroundFileVersion(Node presetManagerNode) {
		Node fileVersion = presetManagerNode.getAttributes().getNamedItem("file-version");
		if (fileVersion != null)
			currentFileVersion = Integer.parseInt(fileVersion.getNodeValue());
	}

	public int getCurrentFileVersion() {
		return currentFileVersion;
	}

	public void onPresetSelectionChanged(Preset newPresetSelection) {
		if (PresetInfoDialog.isShown())
			PresetInfoDialog.update(newPresetSelection);

		if (LocalSettingsProvider.get().getSettings().selectionAutoScroll
				.isOneOf(SetupModel.SelectionAutoScroll.parameter_and_preset, SetupModel.SelectionAutoScroll.preset))
			scrollToSelectedPreset();
	}

	private void scrollToSelectedPreset() {
		Preset p = getSelectedPreset();

		if (p != null) {
			if (p.getNonPosition().getHeight() == 0) {
				// preset is not yet layouted, wait for layout, then scroll
				requestLayout();
				scrollToSelectedPresetScheduled = true;
			} else {
				p.scrollToMakeFullyVisible();
			}
		}

	}

	private boolean updateBanks(Node banks) {
		boolean banksChanged = false;

		clearDockingRelations();

		ArrayList<MapsControl> currentChildren = new ArrayList<MapsControl>();

		for (MapsControl c : getChildren()) {
			if (c instanceof Bank)
				currentChildren.add((Bank) c);
		}

		selectedBank = banks.getAttributes().getNamedItem("selected-bank").getNodeValue();
		NodeList bankList = banks.getChildNodes();

		for (int i = 0; i < bankList.getLength(); i++) {
			Node bank = bankList.item(i);
			if (bank.getNodeName().equals("preset-bank")) {
				Renameable ui = updateBank(bank);
				banksChanged = ServerProxy.didChange(bank) || banksChanged;
				currentChildren.remove((MapsControl) ui);
			}
		}

		removeObsoleteBanks(currentChildren);
		installDockingRelations();

		if (BankInfoDialog.isShown())
			BankInfoDialog.update();

		return banksChanged;
	}

	private void clearDockingRelations() {
		for (MapsControl control : getChildren()) {
			if (control instanceof Bank) {
				Bank bank = (Bank) control;
				bank.removeAllSlaves();
				bank.removeMaster();
			}
		}
	}

	private void installDockingRelations() {
		for (MapsControl control : getChildren()) {
			if (control instanceof Bank) {
				Bank bank = (Bank) control;
				Bank masterBank = NonMaps.theMaps.getNonLinearWorld().getPresetManager().findBank(bank.getMasterUUID());

				if (masterBank != null) {
					bank.installRelationshipMasterSlave(masterBank);
				}
			}
		}
	}

	public void toggleHiddenBanks() {
		setVisible(!isIntendedVisible());
	}

	private void removeObsoleteBanks(ArrayList<MapsControl> currentChildren) {
		boolean needsLayout = false;

		for (MapsControl control : currentChildren) {
			removeChild(control);
			needsLayout = true;
		}

		if (needsLayout)
			requestLayout();
	}

	private Renameable updateBank(Node bank) {
		String uuid = bank.getAttributes().getNamedItem("uuid").getNodeValue();
		Bank bankUI = findBank(uuid);

		if (bankUI == null) {
			bankUI = addChild(new Bank(this, uuid));
			Updater updater = new Updater(bankUI);
			updater.update(bank, true);
			requestLayout();
		} else if (ServerProxy.didChange(bank)) {
			Updater updater = new Updater(bankUI);
			updater.update(bank, false);
		}
		return bankUI;
	}

	public Bank findBank(String uuid) {
		for (Control c : getChildren()) {
			if (c instanceof Bank) {
				Bank b = (Bank) c;
				if (b.getUUID().equals(uuid))
					return b;
			}
		}
		return null;
	}

	public String getSelectedBank() {
		if (hasCustomPresetSelection())
			return getCustomPresetSelection().getSelectedBank().getUUID();

		return selectedBank;
	}

	public void pushBankOntoTop(Bank presetBank) {
		getChildren().remove(presetBank);
		addChild(presetBank);
	}

	@Override
	public <T extends MapsControl> T addChild(T child) {
		T ret = super.addChild(child);

		if (moveAllBanks != null) {
			getChildren().remove(moveAllBanks);
			super.addChild(moveAllBanks);
		}

		return ret;
	}

	public void selectBank(String bankUUID, boolean userInteraction) {
		if (hasCustomPresetSelection()) {
			getCustomPresetSelection().setSelectedBank(findBank(bankUUID));
			return;
		}

		if (!selectedBank.equals(bankUUID)) {
			selectedBank = bankUUID;
			invalidate(INVALIDATION_FLAG_UI_CHANGED);

			if (userInteraction)
				getNonMaps().getServerProxy().selectBank(bankUUID);

			if (PresetInfoDialog.isShown())
				PresetInfoDialog.update();

			if (BankInfoDialog.isShown())
				BankInfoDialog.update();
		}
	}

	@Override
	public void doFirstLayoutPass(double levelOfDetail) {
		for (MapsControl c : getChildren())
			c.doFirstLayoutPass(levelOfDetail);

		for (Control c : getChildren()) {
			if (c instanceof Bank) {
				Bank b = (Bank) c;
				if (b.getMaster() == null) {
					b.layoutSlaves();
				}
			}
		}
	}

	@Override
	public void doSecondLayoutPass(double parentsWidthFromFirstPass, double parentsHeightFromFirstPass) {
		setNonSize(parentsWidthFromFirstPass, parentsHeightFromFirstPass);
		getNonPosition().moveTo(-parentsWidthFromFirstPass / 2, -parentsHeightFromFirstPass / 2);

		if (moveAllBanks != null)
			moveAllBanks.getNonPosition().set(getAllBanksOutline());
	}

	@Override
	public Control drag(Rect r, DragProxy dragProxy) {
		if (dragProxy.getOrigin() instanceof Bank) {
			return this;
		}

		if (dragProxy.getOrigin() instanceof IPreset) {
			return this;
		}

		if (dragProxy.getOrigin() instanceof EditBufferDraggingButton) {
			return this;
		}

		return super.drag(r, dragProxy);
	}

	public void updateMultipleRectangle(Position pos) {
		moveSomeBanks.update(pos);

		for (Control c : getChildren()) {

			if (c instanceof Bank) {
				Bank b = (Bank) c;
				for (Control bc : b.getPresetList().getChildren()) {
					if (bc instanceof Preset) {
						Preset p = (Preset) bc;
						if (moveSomeBanks.getSelectionRect().intersects(p.getPixRect())) {
							multiSelection.add(p);
						} else {
							multiSelection.remove(p);
						}
					}
				}
			}
		}

		invalidate(INVALIDATION_FLAG_ANIMATION_PROGRESS);
	}

	public void startMultipleRectangle(Position pos) {
		if (multiSelection == null)
			startMultiSelectionEmpty();
		moveSomeBanks = new MoveSomeBanksLayer(this, pos);
		invalidate(INVALIDATION_FLAG_ANIMATION_PROGRESS);
	}

	public boolean hasMultipleRectangle() {
		return moveSomeBanks != null;
	}

	public void endMultipleRectangle() {
		moveSomeBanks = null;
		invalidate(INVALIDATION_FLAG_ANIMATION_PROGRESS);
	}

	@Override
	public Control drop(Position pos, DragProxy dragProxy) {
		if (dragProxy.getOrigin() instanceof Bank) {

			Bank b = (Bank) dragProxy.getOrigin();

			if (b.getMaster() != null)
				b.undockBank();

			NonPosition np = toNonPosition(dragProxy.getPixRect().getPosition());
			np.snapTo(getSnapGridResolution());
			b.getNonPosition().moveTo(np);
			b.requestLayout();
			getNonMaps().getServerProxy().onBankPositionChanged(b);

			return this;
		} else if (dragProxy.getOrigin() instanceof IPreset) {
			IPreset p = (IPreset) dragProxy.getOrigin();

			if (p instanceof Preset) {
				Preset preset = (Preset) p;
				Rect intersection = dragProxy.getPixRect().getIntersection(preset.getParent().getPixRect());
				if (!intersection.isEmpty(0)) {
					return this;
				}
			}

			if (hasMultiplePresetSelection()) {
				getNonMaps().getServerProxy().createNewBankFromPresets(toNonPosition(pos),
						getMultiSelection().getCSV());

			} else {
				getNonMaps().getServerProxy().createNewBankFromPreset(toNonPosition(pos), p);
			}

			return this;
		} else if (dragProxy.getOrigin() instanceof EditBufferDraggingButton) {
			getNonMaps().getServerProxy().newBankFromEditBuffer(toNonPosition(pos));
		}
		return null;
	}

	public boolean canNext() {
		if (hasCustomPresetSelection())
			return getCustomPresetSelection().canNext();

		Preset p = findSelectedPreset();
		if (p != null) {
			Bank b = p.getParent();
			if (b != null)
				return !b.getPresetList().isLast(p);
		}
		return false;
	}

	public boolean canPrev() {
		if (hasCustomPresetSelection())
			return getCustomPresetSelection().canPrev();

		Preset p = findSelectedPreset();
		if (p != null) {
			Bank b = p.getParent();
			if (b != null)
				return !b.getPresetList().isFirst(p);
		}
		return false;
	}

	public LoadToPartMode getLoadToPartMode() {
		return (LoadToPartMode) customPresetSelector;
	}

	public boolean isInLoadToPartMode() {
		return customPresetSelector instanceof LoadToPartMode;
	}

	public Preset findLoadedPreset() {
		for (Control c : getChildren()) {
			if (c instanceof Bank) {
				Bank b = (Bank) c;
				Preset p = b.getPresetList().findLoadedPreset();
				if (p != null)
					return p;
			}
		}
		return null;
	}

	public void handleUpKey() {
		if (getMultiSelection() == null) {
			selectPreviousPreset();
		} else {
			getMultiSelection().handleUpKey();
		}
	}

	private void handleDownKey() {
		if (getMultiSelection() == null) {
			selectNextPreset();
		} else {
			getMultiSelection().handleDownKey();
		}
	}

	@Override
	public Control onKey(final KeyDownEvent event) {

		int keyCode = event.getNativeEvent().getKeyCode();

		if (keyCode == com.google.gwt.event.dom.client.KeyCodes.KEY_RIGHT) {
			selectNextBank(true);
		} else if (keyCode == com.google.gwt.event.dom.client.KeyCodes.KEY_LEFT) {
			selectPreviousBank(true);
		} else if (keyCode == com.google.gwt.event.dom.client.KeyCodes.KEY_DOWN) {
			handleDownKey();
		} else if (keyCode == com.google.gwt.event.dom.client.KeyCodes.KEY_UP) {
			handleUpKey();
		} else if (keyCode == com.google.gwt.event.dom.client.KeyCodes.KEY_ENTER) {
			loadSelectedPreset();
		} else if (keyCode == com.google.gwt.event.dom.client.KeyCodes.KEY_P) {
			if (getNonMaps().getNonLinearWorld().isCtrlDown()) {
				toggleHiddenBanks();
			} else {
				PresetInfoDialog.toggle();
			}
		} else if (keyCode == com.google.gwt.event.dom.client.KeyCodes.KEY_DELETE) {
			PresetManager pm = NonMaps.get().getNonLinearWorld().getPresetManager();
			PresetContextMenu.deletePresetWithBankModal(pm.hasMultiplePresetSelection(), pm, getSelectedPreset());
		} else if (keyCode == com.google.gwt.event.dom.client.KeyCodes.KEY_Z
				&& NonMaps.get().getNonLinearWorld().isCtrlDown()) {
			NonMaps.get().getServerProxy().undo();
		} else if (keyCode == com.google.gwt.event.dom.client.KeyCodes.KEY_Y
				&& NonMaps.get().getNonLinearWorld().isCtrlDown()) {
			NonMaps.get().getServerProxy().redo();
		} else if (keyCode == com.google.gwt.event.dom.client.KeyCodes.KEY_F) {
			PresetSearchDialog.toggle();
		} else if (keyCode == com.google.gwt.event.dom.client.KeyCodes.KEY_U) {
			getNonMaps().getNonLinearWorld().getViewport().getOverlay().getUndoTree().toggle();
		} else if (keyCode == com.google.gwt.event.dom.client.KeyCodes.KEY_B) {
			BankInfoDialog.toggle();
		} else if (keyCode == com.google.gwt.event.dom.client.KeyCodes.KEY_I
				&& NonMaps.get().getNonLinearWorld().isCtrlDown() == false) {
			ParameterInfoDialog.toggle();
		} else if (keyCode == com.google.gwt.event.dom.client.KeyCodes.KEY_H
				&& NonMaps.get().getNonLinearWorld().isCtrlDown()) {
			Window.open("/online-help/index.html", "", "");
		} else if (keyCode == com.google.gwt.event.dom.client.KeyCodes.KEY_ESCAPE) {
			NonMaps.get().getNonLinearWorld().getViewport().getOverlay().removeExistingContextMenus();
			NonMaps.get().getNonLinearWorld().getViewport().getOverlay().collapseGlobalMenu();
			getNonMaps().getNonLinearWorld().getViewport().getOverlay().cancelDragging();
			PointerState.get().removeReceiver();
			closeMultiSelection();
		} else if (keyCode == com.google.gwt.event.dom.client.KeyCodes.KEY_M
				&& NonMaps.get().getNonLinearWorld().isCtrlDown()) {
			Window.open("/NonMaps/MCView/index.html", "", "");
		} else if(keyCode == com.google.gwt.event.dom.client.KeyCodes.KEY_ONE || keyCode == com.google.gwt.event.dom.client.KeyCodes.KEY_NUM_ONE) {
			EditBufferUseCases.get().selectVoiceGroup(VoiceGroup.I);
		} else if(keyCode == com.google.gwt.event.dom.client.KeyCodes.KEY_TWO || keyCode == com.google.gwt.event.dom.client.KeyCodes.KEY_NUM_TWO) {
			EditBufferUseCases.get().selectVoiceGroup(VoiceGroup.II);
		} else {
			return null;
		}

		NonMaps.get().getNonLinearWorld().getViewport().getOverlay().refreshGlobalMenu();
		return this;
	}

	public void selectPreviousPreset() {
		if (hasCustomPresetSelection()) {
			getCustomPresetSelection().selectPreviousPreset();
			return;
		}

		Bank b = findBank(getSelectedBank());
		if (b != null)
			b.getPresetList().selectPrev();
	}

	public void selectNextPreset() {
		if (hasCustomPresetSelection()) {
			getCustomPresetSelection().selectNextPreset();
			return;
		}

		Bank b = findBank(getSelectedBank());
		if (b != null)
			b.getPresetList().selectNext();
	}

	public boolean hasSelectedBank() {
		return findBank(getSelectedBank()) != null;
	}

	public boolean hasSelectedPreset() {
		Bank b = findBank(getSelectedBank());

		if (b != null)
			return b.getPresetList().hasSelectedPreset();

		return false;
	}

	public void selectPreviousBank(boolean userInteraction) {
		if (hasCustomPresetSelection())
			getCustomPresetSelection().selectPreviousBank();
		else
			selectBankWithOrdernumberOffset(-1);
	}

	public void selectNextBank(boolean userInteraction) {
		if (hasCustomPresetSelection())
			getCustomPresetSelection().selectNextBank();
		else
			selectBankWithOrdernumberOffset(1);
	}

	public boolean canSelectPreviousBank() {
		if (hasCustomPresetSelection())
			return getCustomPresetSelection().canSelectPreviousBank();

		String sel = getSelectedBank();
		Bank b = findBank(sel);
		return canSelectBankWithOrdernumberOffset(b, -1);
	}

	public boolean canSelectNextBank() {
		if (hasCustomPresetSelection())
			return getCustomPresetSelection().canSelectNextBank();

		String sel = getSelectedBank();
		Bank b = findBank(sel);
		return canSelectBankWithOrdernumberOffset(b, 1);
	}

	public boolean canSelectBankWithOrdernumberOffset(Bank b, int i) {
		if (b != null) {
			int targetOrderNumber = b.getOrderNumber() + i;

			for (Control c : getChildren()) {
				if (c instanceof Bank) {
					Bank candidate = (Bank) c;
					if (candidate.getOrderNumber() == targetOrderNumber)
						return true;
				}
			}
		}
		return false;
	}

	protected void selectBankWithOrdernumberOffset(int off) {
		String sel = getSelectedBank();
		Bank b = findBank(sel);

		if (b == null)
			selectBankWithOrderNumber(1);
		else
			selectBankWithOrderNumber(b.getOrderNumber() + off);
	}

	private void selectBankWithOrderNumber(int i) {
		for (Control c : getChildren()) {
			if (c instanceof Bank) {
				Bank b = (Bank) c;
				if (b.getOrderNumber() == i) {
					b.selectBank(true);
					return;
				}
			}
		}
	}

	public void loadSelectedPreset() {
		Preset p = findSelectedPreset();

		if (p != null)
			p.load();
	}

	public void loadSelectedPresetPart() {
		if (isInLoadToPartMode()) {
			LoadToPartMode selection = getLoadToPartMode();
			VoiceGroup currentVoiceGroup = EditBufferModel.get().voiceGroup.getValue();
			EditBufferUseCases.get().loadPresetPartIntoPart(selection.getSelectedPreset().getUUID(),
					selection.getSelectedPart(), currentVoiceGroup);
		}
	}

	public Preset findSelectedPreset() {

		if (isInLoadToPartMode()) {
			return getLoadToPartMode().getSelectedPreset();
		}

		Bank b = findBank(getSelectedBank());
		if (b != null)
			return b.getPresetList().findPreset(b.getPresetList().getSelectedPreset());

		return null;
	}

	public LinkedList<Preset> collectMatchingPresets() {
		Set<String> matches = PresetSearch.get().results.getValue();
		LinkedList<Preset> ret = new LinkedList<Preset>();

		for (Control c : getChildren()) {
			if (c instanceof Bank) {
				Bank b = (Bank) c;

				for (Control f : b.getPresetList().getChildren()) {
					if (f instanceof Preset) {
						Preset p = (Preset) f;
						if (matches.contains(p.getUUID()))
							ret.add(p);
					}
				}
			}
		}
		return ret;
	}

	public void zoomToAllFilterMatches() {
		if (PresetSearch.get().zoomToMatches.isTrue() && PresetSearch.get().searchActive.isTrue()) {

			double minX = Double.MAX_VALUE;
			double minY = Double.MAX_VALUE;
			double maxX = -Double.MAX_VALUE;
			double maxY = -Double.MAX_VALUE;
			double presetsHeight = 0;

			for (Preset p : collectMatchingPresets()) {
				minX = Math.min(minX, p.getPixRect().getLeft());
				maxX = Math.max(maxX, p.getPixRect().getRight());
				minY = Math.min(minY, p.getPixRect().getTop());
				maxY = Math.max(maxY, p.getPixRect().getBottom());
				presetsHeight = p.getNonPosition().getHeight();
			}

			if (minX != Double.MAX_VALUE && minY != Double.MAX_VALUE && maxX != Double.MAX_VALUE
					&& maxY != Double.MAX_VALUE) {
				NonPosition leftTop = toNonPosition(new Position(minX, minY));
				NonPosition rightBottom = toNonPosition(new Position(maxX, maxY));
				NonDimension dim = new NonDimension(rightBottom.getX() - leftTop.getX(),
						rightBottom.getY() - leftTop.getY());
				NonRect r = new NonRect(leftTop, dim);

				double minHeight = 7 * presetsHeight;

				if (dim.getHeight() < minHeight)
					r.enlargeToHeight(minHeight);
				NonMaps.theMaps.getNonLinearWorld().zoomTo(r, true);
			}
		}
	}

	public boolean isEmpty() {
		for (Control c : getChildren())
			if (c instanceof Bank)
				if (!((Bank) c).isEmpty())
					return false;

		return true;
	}

	public Preset getSelectedPreset() {
		return findSelectedPreset();
	}

	public boolean getPresetsVisible() {
		return isVisible();
	}

	public MultiplePresetSelection getMultiSelection() {
		return multiSelection;
	}

	public boolean hasMultiplePresetSelection() {
		return multiSelection != null;
	}

	public MultiplePresetSelection startMultiSelection(Preset p, boolean shiftModifier) {
		if (shiftModifier)
			multiSelection = new ShiftMultiplePresetSelection(p);
		else
			multiSelection = new MultiplePresetSelection(p);
		return getMultiSelection();
	}

	private MultiplePresetSelection startMultiSelectionEmpty() {
		multiSelection = new MultiplePresetSelection();
		return getMultiSelection();
	}

	public void closeMultiSelection() {
		if (hasMultiplePresetSelection()) {
			multiSelection = null;
			requestLayout();
			invalidate(INVALIDATION_FLAG_UI_CHANGED);
		}
	}

	public Preset findPreset(String uuid) {
		for (Control c : getChildren()) {
			if (c instanceof Bank) {
				Bank b = (Bank) c;
				Preset p = b.getPresetList().findPreset(uuid);
				if (p != null)
					return p;
			}
		}

		return null;
	}

	@Override
	public Control click(Position eventPoint) {

		if (moveAllBanks != null) {
			toggleMoveAllBanks();
			return this;
		}

		if (multiSelection != null) {
			closeMultiSelection();
			return this;
		}
		return super.click(eventPoint);
	}

	public void toggleMoveAllBanks() {
		if (moveAllBanks != null) {
			removeChild(moveAllBanks);
			moveAllBanks = null;
		} else {
			super.addChild(moveAllBanks = new MoveAllBanksLayer(this));
		}
		requestLayout();
	}

	public boolean isInMoveAllBanks() {
		return moveAllBanks != null;
	}

	public void moveAllBanksBy(NonDimension distance) {

		for (Control c : getChildren()) {
			if (c instanceof Bank) {
				Bank b = (Bank) c;
				NonPosition np = b.getNonPosition().getPosition().copy();
				np.moveBy(distance);
				np.snapTo(getSnapGridResolution());
				b.getNonPosition().moveTo(np);
			}
		}
		requestLayout();
	}

	public static int getSnapGridResolution() {
		return 15;
	}

	public void setAttachingTapes(Tape one, Tape other) {
		if (attachingTapes[0] != one) {
			attachingTapes[0] = one;
			invalidate(INVALIDATION_FLAG_UI_CHANGED);
		}

		if (attachingTapes[1] != other) {
			attachingTapes[1] = other;
			invalidate(INVALIDATION_FLAG_UI_CHANGED);
		}
	}

	public boolean isAttachingTape(Tape inQuestion) {
		return attachingTapes[0] == inQuestion || attachingTapes[1] == inQuestion;
	}

	public void resetAttachingTapes() {
		setAttachingTapes(null, null);
	}

	public String getSelectedPresetUUID() {
		Preset p = getSelectedPreset();
		if (p != null)
			return p.getUUID();

		return "";
	}

	public Preset getLoadedPreset() {
		return findLoadedPreset();
	}

	public boolean isChangingPresetWhileInDirectLoad() {
		boolean directLoadActive = PresetManagerPresenterProvider.get().getPresenter().loadModeButtonState;

		if (!directLoadActive)
			return false;

		Preset loadedPreset = findLoadedPreset();
		boolean isInitSound = loadedPreset == null;

		if (isInitSound)
			return false;

		return findSelectedPreset() != loadedPreset;
	}

	public CustomPresetSelector getCustomPresetSelection() {
		return customPresetSelector;
	}

	public boolean isInStoreSelectMode() {
		return customPresetSelector instanceof StoreSelectMode;
	}

	public StoreSelectMode getStoreSelectMode() {
		if (isInStoreSelectMode())
			return (StoreSelectMode) customPresetSelector;
		return null;
	}
};
