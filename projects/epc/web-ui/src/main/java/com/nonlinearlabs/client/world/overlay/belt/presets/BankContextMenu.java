package com.nonlinearlabs.client.world.overlay.belt.presets;

import com.google.gwt.http.client.URL;
import com.google.gwt.user.client.Window;
import com.nonlinearlabs.client.ClipboardManager.ClipboardContent;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.Renameable;
import com.nonlinearlabs.client.presenters.BankPresenterProviders;
import com.nonlinearlabs.client.presenters.DeviceSettingsProvider;
import com.nonlinearlabs.client.presenters.PresetManagerPresenterProvider;
import com.nonlinearlabs.client.useCases.BankUseCases;
import com.nonlinearlabs.client.useCases.ClipboardUseCases;
import com.nonlinearlabs.client.useCases.EditBufferUseCases;
import com.nonlinearlabs.client.useCases.PresetManagerUseCases;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.RenameDialog;
import com.nonlinearlabs.client.world.maps.NonPosition;
import com.nonlinearlabs.client.world.overlay.BankInfoDialog;
import com.nonlinearlabs.client.world.overlay.ContextMenu;
import com.nonlinearlabs.client.world.overlay.ContextMenuItem;
import com.nonlinearlabs.client.world.overlay.OverlayLayout;
import com.nonlinearlabs.client.world.overlay.belt.presets.TextUpload.TextUploadedHandler;

public abstract class BankContextMenu extends ContextMenu {

	protected String uuid = "";

	@Override
	public void fadeOut() {
		uuid = "";
		super.fadeOut();
	}

	public BankContextMenu(OverlayLayout parent, final String uuid) {
		super(parent);
		this.uuid = uuid;

		var bankPresenter = BankPresenterProviders.get().getPresenter(uuid);
		var pmPresenter = PresetManagerPresenterProvider.get().getPresenter();

		if (hasBankCreationRights()) {
			addChild(new ContextMenuItem(this, "Create New Bank") {
				@Override
				public Control click(Position eventPoint) {
					NonPosition pos = getNonMaps().getNonLinearWorld().toNonPosition(eventPoint);
					PresetManagerUseCases.get().createNewBank(pos);
					return super.click(eventPoint);
				}
			});

			if (getNonMaps().getNonLinearWorld().getClipboardManager().getClipboardState() != ClipboardContent.empty) {
				addChild(new ContextMenuItem(this, "Paste") {
					@Override
					public Control click(Position eventPoint) {
						NonPosition pos = getNonMaps().getNonLinearWorld()
								.toNonPosition(getParent().getPixRect().getLeftTop());
						getNonMaps().getServerProxy().pasteOnPresetManager(pos);
						return super.click(eventPoint);
					}
				});
			}

			addChild(new ContextMenuItem(this, "Import Bank from File ...") {
				@Override
				public Control click(final Position eventPoint) {
					new TextUpload(new TextUploadedHandler() {
						@Override
						public void onTextUploaded(String fileName, String text) {
							NonPosition pos = getNonMaps().getNonLinearWorld()
									.toNonPosition(getParent().getPixRect().getLeftTop());
							NonMaps.theMaps.getServerProxy().importBank(fileName, text, pos);
						}
					}, ".xml");
					return super.click(eventPoint);
				}
			});

		}


		if (!uuid.isEmpty()) {
			if (DeviceSettingsProvider.get().getPresenter().externalMidiEnabled) {
				if (!bankPresenter.isMidiBank) {
					addChild(new ContextMenuItem(this, "Select Bank as Midi PC Receiver") {
						@Override
						public Control click(Position eventPoint) {
							EditBufferUseCases.get().selectMidiBank(uuid);
							return super.click(eventPoint);
						}
					});
				} else {
					addChild(new ContextMenuItem(this, "Remove Bank as Midi PC Receiver") {
						@Override
						public Control click(Position eventPoint) {
							EditBufferUseCases.get().selectMidiBank(null);
							return super.click(eventPoint);
						}
					});
				}
			}

			addChild(new ContextMenuItem(this, "Export Bank as File ...") {
				@Override
				public Control click(Position eventPoint) {
					String bankName = URL.encodePathSegment(bankPresenter.name);
					String uri = "/presets/banks/download-bank/" + bankName + ".xml?uuid=" + bankPresenter.uuid;
					Window.open(uri, "", "");
					return super.click(eventPoint);
				}
			});

			if (!BankInfoDialog.isShown()) {
				String bankInfoText = "Bank Info ...";
				addChild(new ContextMenuItem(this, bankInfoText) {
					@Override
					public Control click(Position eventPoint) {
						PresetManagerUseCases.get().selectBank(uuid);
						BankInfoDialog.toggle();
						invalidate(INVALIDATION_FLAG_UI_CHANGED);
						return super.click(eventPoint);
					}
				});
			}

			addChild(new ContextMenuItem(this, "Rename ...") {
				@Override
				public Control click(Position eventPoint) {
					RenameDialog.open(new Renameable() {

						@Override
						public String getCurrentName() {
							return bankPresenter.name;
						}

						@Override
						public String getTitleName() {
							return bankPresenter.name;
						}

						@Override
						public String getEntityName() {
							return "Bank";
						}

						@Override
						public void setName(String newName) {
							BankUseCases.get().rename(uuid, newName);
						}

					});
					return super.click(eventPoint);
				}
			});

			addChild(new ContextMenuItem(this, "Copy") {
				@Override
				public Control click(Position eventPoint) {
					ClipboardUseCases.get().copyBank(uuid);
					return super.click(eventPoint);
				}
			});

			if (hasPaste()) {
				if (getNonMaps().getNonLinearWorld().getClipboardManager()
						.getClipboardState() != ClipboardContent.empty) {
					addChild(new ContextMenuItem(this, "Paste") {
						@Override
						public Control click(Position eventPoint) {
							ClipboardUseCases.get().pasteOnBank(uuid);
							return super.click(eventPoint);
						}
					});
				}
			}

			if (bankPresenter.orderNumber > 1) {
				addChild(new ContextMenuItem(this, "Move Left") {
					@Override
					public Control click(Position eventPoint) {
						PresetManagerUseCases.get().moveBankBy(uuid, "LeftByOne");
						return super.click(eventPoint);
					}
				});
			}

			if (bankPresenter.orderNumber < pmPresenter.banks.size()) {
				addChild(new ContextMenuItem(this, "Move Right") {
					@Override
					public Control click(Position eventPoint) {
						PresetManagerUseCases.get().moveBankBy(uuid, "RightByOne");
						return super.click(eventPoint);
					}
				});
			}

			addChild(new ContextMenuItem(this, "Delete") {
				@Override
				public Control click(Position eventPoint) {
					PresetManagerUseCases.get().deleteBank(uuid);
					return super.click(eventPoint);
				}
			});

			addChild(new ContextMenuItem(this, "Export as File ...") {
				@Override
				public Control click(Position eventPoint) {
					String bankName = URL.encodePathSegment(bankPresenter.name);
					String uri = "/presets/banks/download-bank/" + bankName + ".xml?uuid=" + bankPresenter.uuid;
					Window.open(uri, "", "");
					return super.click(eventPoint);
				}
			});

			if (DeviceSettingsProvider.get().getPresenter().externalMidiEnabled) {
				if (!bank.isMidiBank()) {
					addChild(new ContextMenuItem(this, "Connect Bank to MIDI PC") {
						@Override
						public Control click(Position eventPoint) {
							EditBufferUseCases.get().selectMidiBank(bank);
							return super.click(eventPoint);
						}
					});
				} else {
					addChild(new ContextMenuItem(this, "Disconnect Bank from MIDI PC") {
						@Override
						public Control click(Position eventPoint) {
							EditBufferUseCases.get().selectMidiBank(null);
							return super.click(eventPoint);
						}
					});
				}
			}

			addChild(new ContextMenuItem(this, "Sort Bank Numbers") {
				@Override
				public Control click(Position eventPoint) {
					getNonMaps().getServerProxy().sortBankNumbers();
					return super.click(eventPoint);
				}
			});
		}
	}

	protected abstract boolean hasCollapse();

	protected abstract boolean hasPaste();

	protected abstract boolean hasBankCreationRights();

	public String getBank() {
		return uuid;
	}
}
