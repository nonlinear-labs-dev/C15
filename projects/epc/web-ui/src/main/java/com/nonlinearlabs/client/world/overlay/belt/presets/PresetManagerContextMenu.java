package com.nonlinearlabs.client.world.overlay.belt.presets;

import com.google.gwt.core.client.JavaScriptObject;
import com.google.gwt.dom.client.NativeEvent;
import com.google.gwt.event.dom.client.ChangeEvent;
import com.google.gwt.event.dom.client.ChangeHandler;
import com.google.gwt.user.client.Window;
import com.google.gwt.user.client.Window.Navigator;
import com.google.gwt.user.client.ui.FileUpload;
import com.google.gwt.user.client.ui.RootPanel;
import com.nonlinearlabs.client.ClipboardManager.ClipboardContent;
import com.nonlinearlabs.client.dataModel.presetManager.PresetManagerModel;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.Renameable;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.RenameDialog;
import com.nonlinearlabs.client.world.maps.NonPosition;
import com.nonlinearlabs.client.world.maps.presets.PresetManager;
import com.nonlinearlabs.client.world.overlay.ContextMenu;
import com.nonlinearlabs.client.world.overlay.ContextMenuItem;
import com.nonlinearlabs.client.world.overlay.OverlayLayout;
import com.nonlinearlabs.client.world.maps.presets.bank.Bank;
import com.google.gwt.http.client.URL;

public class PresetManagerContextMenu extends ContextMenu {

	public PresetManagerContextMenu(OverlayLayout parent) {
		super(parent);

		addChild(new ContextMenuItem(this, "Create New Bank") {
			@Override
			public Control click(Position eventPoint) {
				NonPosition pos = getNonMaps().getNonLinearWorld().toNonPosition(getParent().getPixRect().getLeftTop());
				createNewBank(pos);
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

				new TextUpload((fileName, text) -> {
					NonPosition pos = getNonMaps().getNonLinearWorld()
							.toNonPosition(getParent().getPixRect().getLeftTop());
					NonMaps.theMaps.getServerProxy().importBank(fileName, text, pos);
				}, ".xml");

				return super.click(eventPoint);
			}
		});


		PresetManager pm = NonMaps.get().getNonLinearWorld().getPresetManager();

		if(!pm.getBanks().isEmpty()) {
			addChild(new ContextMenuItem(this, "Save all Banks as Backup File ...") {
				@Override
				public Control click(Position eventPoint) {
					String uri = "/presets/download-banks";
					Window.open(uri, "", "");
					return super.click(eventPoint);
				}
			});
		}

		addChild(new ContextMenuItem(this, "Restore all Banks from Backup File ...") {
			@Override
			public Control click(Position eventPoint) {

				NonMaps.get().getNonLinearWorld().getViewport().getOverlay().promptUser(
						"This will replace all current banks! Please save the banks with your work as files before restoring the backup.",
						() -> {
							final FileUpload upload = new FileUpload();
							upload.setName("uploadFormElement");

							if (!Navigator.getPlatform().toLowerCase().contains("mac"))
								upload.getElement().setAttribute("accept", ".xml.tar.gz");

							upload.addChangeHandler(new ChangeHandler() {

								@Override
								public void onChange(ChangeEvent event) {
									loadBackupFile(event.getNativeEvent(), new ZipUploadedHandler() {

										@Override
										public void onZipUploaded(JavaScriptObject buffer) {
											NonMaps.theMaps.getServerProxy().importPresetManager(buffer);
										}
									});

									RootPanel.get().remove(upload);
								}
							});

							upload.click();
							RootPanel.get().add(upload);
						}, () -> {

						});
				return super.click(eventPoint);
			}

		});

		if(!pm.getBanks().isEmpty()) {
			addChild(new ContextMenuItem(this, !pm.isInMoveAllBanks() ? "Move all Banks" : "Disable Move all Banks") {
				@Override
				public Control click(Position eventPoint) {
					getNonMaps().getNonLinearWorld().getPresetManager().toggleMoveAllBanks();
					return super.click(eventPoint);
				}
			});

			if (!pm.areAllBanksCollapsed()) {
				addChild(new ContextMenuItem(this, "Collapse all Banks") {
					@Override
					public Control click(Position eventPoint) {
						pm.setAllBanksCollapseState(true);
						return super.click(eventPoint);
					}
				});
			}

			if (pm.isAnyBankCollapsed()) {
				addChild(new ContextMenuItem(this, "Expand all Banks") {
					@Override
					public Control click(Position eventPoint) {
						pm.setAllBanksCollapseState(false);
						return super.click(eventPoint);
					}
				});
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

	public static void createNewBank(final NonPosition pos) {
		RenameDialog.open(new Renameable() {

			String name = "New bank";

			@Override
			public void setName(String newName) {
				name = newName;
				NonMaps.get().getServerProxy().newBank(name, pos);
			}

			@Override
			public String getEntityName() {
				return "new bank";
			}

			@Override
			public String getCurrentName() {
				return name;
			}

			@Override
			public String getTitleName() {
				return name;
			}
		});
	}

	private interface ZipUploadedHandler {
		void onZipUploaded(JavaScriptObject buffer);
	}

	private native void loadBackupFile(NativeEvent evt, ZipUploadedHandler receiver) /*-{
																						var files = evt.target.files;
																						if (files != null && files.length > 0) {
																						var file = files[0];
																						var reader = new FileReader();
																						
																						reader.onload = function(e) {
																						var zip = reader.result;
																						receiver.@com.nonlinearlabs.client.world.overlay.belt.presets.PresetManagerContextMenu.ZipUploadedHandler::onZipUploaded(Lcom/google/gwt/core/client/JavaScriptObject;)(zip);
																						}
																						reader.readAsArrayBuffer(file);
																						}
																						}-*/;
}
