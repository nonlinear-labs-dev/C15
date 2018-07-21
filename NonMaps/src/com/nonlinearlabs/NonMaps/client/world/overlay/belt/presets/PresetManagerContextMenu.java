package com.nonlinearlabs.NonMaps.client.world.overlay.belt.presets;

import com.google.gwt.core.client.JavaScriptObject;
import com.google.gwt.dom.client.NativeEvent;
import com.google.gwt.event.dom.client.ChangeEvent;
import com.google.gwt.event.dom.client.ChangeHandler;
import com.google.gwt.user.client.Window;
import com.google.gwt.user.client.ui.FileUpload;
import com.google.gwt.user.client.ui.RootPanel;
import com.nonlinearlabs.NonMaps.client.ClipboardManager.ClipboardContent;
import com.nonlinearlabs.NonMaps.client.NonMaps;
import com.nonlinearlabs.NonMaps.client.Renameable;
import com.nonlinearlabs.NonMaps.client.world.Control;
import com.nonlinearlabs.NonMaps.client.world.Position;
import com.nonlinearlabs.NonMaps.client.world.RenameDialog;
import com.nonlinearlabs.NonMaps.client.world.maps.NonPosition;
import com.nonlinearlabs.NonMaps.client.world.overlay.ContextMenu;
import com.nonlinearlabs.NonMaps.client.world.overlay.ContextMenuItem;
import com.nonlinearlabs.NonMaps.client.world.overlay.OverlayLayout;
import com.nonlinearlabs.NonMaps.client.world.overlay.belt.presets.TextUpload.TextUploadedHandler;

public class PresetManagerContextMenu extends ContextMenu {

	public PresetManagerContextMenu(OverlayLayout parent) {
		super(parent);

		addChild(new ContextMenuItem(this, "Create New Bank") {
			@Override
			public Control click(Position eventPoint) {
				NonPosition pos = getNonMaps().getNonLinearWorld().toNonPosition(eventPoint);
				createNewBank(pos);
				return super.click(eventPoint);
			}
		});

		if (getNonMaps().getNonLinearWorld().getClipboardManager().getClipboardState() != ClipboardContent.empty) {
			addChild(new ContextMenuItem(this, "Paste") {
				@Override
				public Control click(Position eventPoint) {
					NonPosition pos = getNonMaps().getNonLinearWorld().toNonPosition(eventPoint);
					getNonMaps().getServerProxy().pasteOnPresetManager(pos);
					return super.click(eventPoint);
				}
			});
		}

		addChild(new ContextMenuItem(this, "Import Bank from File...") {
			@Override
			public Control click(final Position eventPoint) {
				new TextUpload(new TextUploadedHandler() {
					@Override
					public void onTextUploaded(String fileName, String text) {
						NonPosition pos = getNonMaps().getNonLinearWorld().toNonPosition(eventPoint);
						NonMaps.theMaps.getServerProxy().importBank(fileName, text, pos);
					}
				});
				return super.click(eventPoint);
			}
		});

		addChild(new ContextMenuItem(this, "Save all Banks as Backup File...") {
			@Override
			public Control click(Position eventPoint) {
				String uri = "/presets/download-banks";
				Window.open(uri, "", "");
				return super.click(eventPoint);
			}
		});

		addChild(new ContextMenuItem(this, "Restore all Banks from Backup File...") {
			@Override
			public Control click(Position eventPoint) {
				boolean confirm = Window
						.confirm("This will replace all current banks! Please save the banks with your work as files before restoring the backup.");

				if (confirm == false) {
					return null;
				}
				final FileUpload upload = new FileUpload();
				upload.setName("uploadFormElement");
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
				return super.click(eventPoint);
			}
		});

		addChild(new ContextMenuItem(this, "Move all Banks") {
			@Override
			public Control click(Position eventPoint) {
				getNonMaps().getNonLinearWorld().getPresetManager().toggleMoveAllBanks();
				return super.click(eventPoint);
			}
		});
		
		addChild(new ContextMenuItem(this, "Sort Bank Numbers") {
			@Override
			public Control click(Position eventPoint) {
				getNonMaps().getServerProxy().sortBankNumbers();
				return super.click(eventPoint);
			}
		});

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
				receiver.@com.nonlinearlabs.NonMaps.client.world.overlay.belt.presets.PresetManagerContextMenu.ZipUploadedHandler::onZipUploaded(Lcom/google/gwt/core/client/JavaScriptObject;)(zip);
			}
			reader.readAsArrayBuffer(file);
		}
	}-*/;
}
