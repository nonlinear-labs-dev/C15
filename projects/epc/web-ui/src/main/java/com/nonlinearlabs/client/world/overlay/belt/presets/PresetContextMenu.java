package com.nonlinearlabs.client.world.overlay.belt.presets;

import java.util.ArrayList;
import java.util.HashSet;

import com.nonlinearlabs.client.ClipboardManager.ClipboardContent;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.Renameable;
import com.nonlinearlabs.client.dataModel.presetManager.Preset.Color;
import com.nonlinearlabs.client.dataModel.setup.SetupModel;
import com.nonlinearlabs.client.dataModel.setup.SetupModel.BooleanValues;
import com.nonlinearlabs.client.presenters.BankPresenterProviders;
import com.nonlinearlabs.client.presenters.PresetManagerPresenter;
import com.nonlinearlabs.client.presenters.PresetManagerPresenterProvider;
import com.nonlinearlabs.client.presenters.PresetPresenter;
import com.nonlinearlabs.client.presenters.PresetPresenterProviders;
import com.nonlinearlabs.client.useCases.BankUseCases;
import com.nonlinearlabs.client.useCases.ClipboardUseCases;
import com.nonlinearlabs.client.useCases.PresetManagerUseCases;
import com.nonlinearlabs.client.useCases.PresetUseCases;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.RenameDialog;
import com.nonlinearlabs.client.world.overlay.CompareDialog;
import com.nonlinearlabs.client.world.overlay.ContextMenu;
import com.nonlinearlabs.client.world.overlay.ContextMenuItem;
import com.nonlinearlabs.client.world.overlay.Overlay;
import com.nonlinearlabs.client.world.overlay.OverlayLayout;
import com.nonlinearlabs.client.world.overlay.PresetContextMenuItem;
import com.nonlinearlabs.client.world.overlay.PresetInfoDialog;

public class PresetContextMenu extends ContextMenu {

	private String uuid;
	private PresetPresenter presetPresenter;
	private PresetManagerPresenter presetManagerPresenter;

	public PresetContextMenu(OverlayLayout parent, final String uuid) {
		super(parent);
		this.uuid = uuid;

		presetPresenter = PresetPresenterProviders.get().getPresenter(uuid);
		presetManagerPresenter = PresetManagerPresenterProvider.get().getPresenter();

		final boolean multipleSelectionAllowed = SetupModel.get().localSettings.presetDragDrop
				.getValue() == BooleanValues.on;

		if (multipleSelectionAllowed) {
			addChild(new ContextMenuItem(this,
					presetManagerPresenter.multiSelection ? "Disable Multiple Selection" : "Start Multiple Selection") {
				@Override
				public Control click(Position eventPoint) {
					if (presetManagerPresenter.multiSelection)
						PresetManagerUseCases.get().finishMultipleSelection();
					else
						PresetManagerUseCases.get().toggleMultipleSelection(uuid);
					return super.click(eventPoint);
				}
			});
		}

		if (!presetManagerPresenter.multiSelection) {
			if (!PresetInfoDialog.isShown()) {
				String presetInfoText = "Preset Info ...";
				addChild(new PresetContextMenuItem(this, presetInfoText) {

					@Override
					public Control click(Position eventPoint) {
						BankUseCases.get().selectPreset(uuid);
						PresetInfoDialog.toggle();
						invalidate(INVALIDATION_FLAG_UI_CHANGED);
						return super.click(eventPoint);
					}
				});
			}

			addChild(new PresetContextMenuItem(this, "Rename ...") {
				@Override
				public Control click(Position eventPoint) {
					RenameDialog.open(new Renameable() {

						@Override
						public String getCurrentName() {
							return presetPresenter.name;
						}

						@Override
						public String getTitleName() {
							return presetPresenter.name;
						}

						@Override
						public String getEntityName() {
							return "Preset";
						}

						@Override
						public void setName(String newName) {
							PresetUseCases.get().rename(uuid, newName);

						}

					});
					return super.click(eventPoint);
				}
			});

			addChild(new PresetContextMenuItem(this, "Cut") {
				@Override
				public Control click(Position eventPoint) {
					ClipboardUseCases.get().cutPreset(uuid);
					return super.click(eventPoint);
				}
			});

			addChild(new PresetContextMenuItem(this, "Copy") {
				@Override
				public Control click(Position eventPoint) {
					ClipboardUseCases.get().copyPreset(uuid);
					return super.click(eventPoint);
				}
			});

			if (getNonMaps().getNonLinearWorld().getClipboardManager().getClipboardState() != ClipboardContent.empty) {
				addChild(new PresetContextMenuItem(this, "Paste") {
					@Override
					public Control click(Position eventPoint) {
						ClipboardUseCases.get().pasteOnPreset(uuid);
						return super.click(eventPoint);
					}
				});
			}
		} else

		{
			addChild(new PresetContextMenuItem(this, "Copy") {
				@Override
				public Control click(Position eventPoint) {
					ClipboardUseCases.get().copyPresets();
					return super.click(eventPoint);
				}
			});
		}

		addChild(new PresetContextMenuItem(this, "Delete") {
			@Override
			public Control click(Position eventPoint) {
				deletePresetWithBankModal(uuid);
				return super.click(eventPoint);
			}
		});

		if (presetManagerPresenter.numSelectedPresetsInMultiSelection == 2) {
			addChild(new PresetContextMenuItem(this, "Compare ...") {
				@Override
				public Control click(Position eventPoint) {
					ArrayList<String> selPresets = presetManagerPresenter.currentMultiSelection;
					CompareDialog.open(selPresets.get(0), selPresets.get(1));
					return super.click(eventPoint);
				}
			});
		} else if (!presetManagerPresenter.multiSelection) {
			addChild(new PresetContextMenuItem(this, "Compare to Editbuffer ...") {
				@Override
				public Control click(Position eventPoint) {
					CompareDialog.open(uuid);
					return super.click(eventPoint);
				}
			});
		}

		addChild(new TwoPartContextMenuItem(this, "Color Tag", "\uE01A") {
			@Override
			public Control click(Position eventPosition) {
				Overlay o = NonMaps.theMaps.getNonLinearWorld().getViewport().getOverlay();
				return o.setContextMenu(calcColorMenuPosition(this), new PresetColorTagContextMenu(o, uuid));
			}
		});
	}

	protected Position calcColorMenuPosition(TwoPartContextMenuItem item) {
		double bottom = item.getPixRect().getBottom();
		double right = item.getPixRect().getRight() + 5;
		double height = item.getPixRect().getHeight() * Color.values().length;
		return new Position(right, bottom - height);
	}

	public static void deletePresetWithBankModal(String uuid) {
		var pm = PresetManagerPresenterProvider.get().getPresenter();
		var uc = PresetManagerUseCases.get();
		if (uc.hasMultipleSelection()) {
			String csv = uc.getSelectedPresetsCSV();
			if (selectionContainsSolePresets(pm.currentMultiSelection)) {
				PresetDeleter.open(csv);
			} else {
				uc.deletePresets(csv, false);
			}

			uc.finishMultipleSelection();
		} else {
			var presetPresenter = PresetPresenterProviders.get().getPresenter(uuid);
			if (presetPresenter.isLastPresetInBank)
				PresetDeleter.open(uuid);
			else
				NonMaps.get().getServerProxy().deletePreset(uuid, false);
		}
	}

	public static boolean selectionContainsSolePresets(ArrayList<String> currentMultiSelection) {
		HashSet<String> banks = new HashSet<String>();

		for (var s : currentMultiSelection) {
			var p = PresetPresenterProviders.get().getPresenter(s);
			banks.add(p.bankUuid);
		}

		for (var u : banks) {
			var b = BankPresenterProviders.get().getPresenter(u);
			if (currentMultiSelection.containsAll(b.presets))
				return true;
		}

		return false;
	}

	public String getPreset() {
		return uuid;
	}
}
