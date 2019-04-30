package com.nonlinearlabs.NonMaps.client.world;

import static com.nonlinearlabs.NonMaps.client.world.Control.INVALIDATION_FLAG_UI_CHANGED;

import com.google.gwt.core.client.Scheduler;
import com.google.gwt.event.dom.client.ClickEvent;
import com.google.gwt.event.dom.client.ClickHandler;
import com.google.gwt.event.dom.client.KeyCodes;
import com.google.gwt.event.logical.shared.CloseEvent;
import com.google.gwt.event.logical.shared.CloseHandler;
import com.google.gwt.user.client.Command;
import com.google.gwt.user.client.Event;
import com.google.gwt.user.client.Event.NativePreviewEvent;
import com.google.gwt.user.client.ui.Button;
import com.google.gwt.user.client.ui.DialogBox;
import com.google.gwt.user.client.ui.FlowPanel;
import com.google.gwt.user.client.ui.PopupPanel;
import com.google.gwt.user.client.ui.RootPanel;
import com.google.gwt.user.client.ui.TextBox;
import com.nonlinearlabs.NonMaps.client.NonMaps;
import com.nonlinearlabs.NonMaps.client.Renameable;
import com.nonlinearlabs.NonMaps.client.dataModel.editBuffer.EditBufferModel;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.ParameterEditor;
import com.nonlinearlabs.NonMaps.client.world.maps.presets.PresetManager;
import com.nonlinearlabs.NonMaps.client.world.maps.presets.bank.preset.Preset;

public class RenameDialog extends DialogBox {

	private static RenameDialog theDialog;

	private TextBox nameField;
	private Renameable renameable;
	private static String presetToWaitFor = "";

	public static void open(Renameable renameable) {
		if (theDialog != null)
			theDialog.hide();

		theDialog = new RenameDialog(renameable);

		RootPanel.get().add(theDialog);
		theDialog.center();
		theDialog.show();

		theDialog.addCloseHandler(new CloseHandler<PopupPanel>() {

			@Override
			public void onClose(CloseEvent<PopupPanel> event) {
				theDialog = null;
				NonMaps.get().captureFocus();
			}
		});
	}

	private RenameDialog(Renameable r) {
		renameable = r;

		getElement().addClassName("rename-dialog");

		String titleName = renameable.getTitleName();

		if (titleName.isEmpty())
			setText("Rename " + renameable.getEntityName());
		else
			setText("Rename " + renameable.getEntityName() + " '" + titleName + "'");

		setAnimationEnabled(true);
		setGlassEnabled(true);

		FlowPanel panel = new FlowPanel();

		nameField = new TextBox();
		nameField.setText(renameable.getCurrentName());
		panel.add(nameField);

		Button ok = new Button("OK");
		ok.addClickHandler(event -> commit());

		Button cancel = new Button("Cancel");
		cancel.addClickHandler(event -> cancel());

		panel.add(ok);
		panel.add(cancel);
		setWidget(panel);

		Scheduler.get().scheduleFinally((Command) () -> {
			nameField.selectAll();
			nameField.setFocus(true);
		});
	}

	public static boolean isPresetBeingRenamed(Preset preset) {
		if (theDialog != null && theDialog.renameable instanceof Preset) {
			Preset p = (Preset) theDialog.renameable;
			return preset == p;
		}
		return false;
	}

	@Override
	protected void onPreviewNativeEvent(NativePreviewEvent event) {
		super.onPreviewNativeEvent(event);
		switch (event.getTypeInt()) {
		case Event.ONKEYDOWN:
			if (event.getNativeEvent().getKeyCode() == KeyCodes.KEY_ESCAPE) {
				hide();
			} else if (event.getNativeEvent().getKeyCode() == KeyCodes.KEY_ENTER) {
				commit();
			}
			break;
		}
	}

	private void commit() {
		renameable.setName(nameField.getText());
		RenameDialog.this.hide();
		NonMaps.get().captureFocus();
	}

	private void cancel() {

		RenameDialog.this.hide();
		NonMaps.get().captureFocus();
		NonMaps.get().getNonLinearWorld().invalidate(INVALIDATION_FLAG_UI_CHANGED);
	}

	public static void onPresetManagerUpdate(PresetManager presetManager) {
		if (!presetToWaitFor.isEmpty()) {
			Preset p = presetManager.findPreset(presetToWaitFor);
			if (p != null) {
				presetToWaitFor = "";
				RenameDialog.open(p);
			}
		}
	}

	public static void awaitNewPreset(String uuid) {
		EditBufferModel editBuffer = EditBufferModel.get();
		boolean isModified = editBuffer.isAnyParamChanged();
		boolean storedInPlace = editBuffer.loadedPreset.getValue().equals(uuid);

		if (isModified && !storedInPlace) {
			presetToWaitFor = uuid;
		} else {
			presetToWaitFor = "";
		}
	}
}
