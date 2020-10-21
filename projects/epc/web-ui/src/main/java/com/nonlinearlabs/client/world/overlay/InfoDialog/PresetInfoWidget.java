package com.nonlinearlabs.client.world.overlay.InfoDialog;

import com.google.gwt.event.dom.client.KeyCodes;
import com.google.gwt.user.client.ui.FlexTable;
import com.google.gwt.user.client.ui.FocusWidget;
import com.google.gwt.user.client.ui.HTMLPanel;
import com.google.gwt.user.client.ui.IntegerBox;
import com.google.gwt.user.client.ui.Label;
import com.google.gwt.user.client.ui.TextArea;
import com.google.gwt.user.client.ui.TextBox;
import com.google.gwt.user.client.ui.Widget;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.dataModel.presetManager.Preset.Color;
import com.nonlinearlabs.client.presenters.BankPresenterProviders;
import com.nonlinearlabs.client.presenters.PresetManagerPresenterProvider;
import com.nonlinearlabs.client.presenters.PresetPresenter;
import com.nonlinearlabs.client.presenters.PresetPresenterProviders;

public class PresetInfoWidget {

	private static PresetInfoWidget instance;
	private TextArea comment;
	private Label deviceName;
	private Label softwareVersion;
	private TextBox name;
	private Label storeTime;
	private Label bankName;
	private IntegerBox position;
	private FocusWidget haveFocus = null;
	private PresetPresenter m_currentShownPreset = null;

	private ColorTagBox colorBox = new ColorTagBox() {
		@Override
		public void onBoxClick(Color c) {
			NonMaps.get().getServerProxy().setPresetAttribute(PresetInfoWidget.get().m_currentShownPreset.uuid, "color",
					c.toString());
		}
	};

	public Widget panel;

	static public PresetInfoWidget get() {
		if (instance == null)
			instance = new PresetInfoWidget();
		return instance;
	}

	public void updateInfo(PresetPresenter presetPresenter, boolean force) {
		if (m_currentShownPreset != presetPresenter) {
			saveContent();
			m_currentShownPreset = presetPresenter;
		}

		if (presetPresenter != null) {
			String presetName = presetPresenter.name;
			deviceName.setText(presetPresenter.deviceName);
			softwareVersion.setText(presetPresenter.softwareVersion);
			storeTime.setText(presetPresenter.storeTime);
			String commentText = presetPresenter.comment;

			if (force || haveFocus != comment) {
				comment.setText(commentText);

				if (comment.getElement().getScrollHeight() > 0) {
					comment.setHeight("1em");
					int height = comment.getElement().getScrollHeight() + 5;
					comment.setHeight(height + "px");
				}
			}

			if (force || haveFocus != name) {
				if (!presetName.equals(name.getText())) {
					name.setText(presetName);
				}
			}

			if (force || haveFocus != position) {
				position.setText(presetPresenter.paddedNumber);
			}

			bankName.setText(presetPresenter.bankName);
			colorBox.updateCurrentHighlight(presetPresenter.color);
		}
	}

	protected PresetInfoWidget() {

		HTMLPanel presetNameAndPositionBox = new HTMLPanel("div", "");
		presetNameAndPositionBox.getElement().addClassName("preset-name-and-pos");
		presetNameAndPositionBox.add(position = new IntegerBox());
		presetNameAndPositionBox.add(name = new TextBox());
		position.getElement().addClassName("position-box editable");
		name.getElement().addClassName("preset-name-box editable");

		FlexTable panel = new FlexTable();
		addRow(panel, "Bank", bankName = new Label());
		addRow(panel, "Position/Name", presetNameAndPositionBox);
		addRow(panel, "Comment", comment = new TextArea());
		addRow(panel, "Color Tag", colorBox.getHTML());
		addRow(panel, "Last Change", storeTime = new Label(""));
		addRow(panel, "Device Name", deviceName = new Label(""));
		addRow(panel, "UI Version", softwareVersion = new Label(""));

		position.getElement().addClassName("gwt-TextBox");

		comment.getElement().addClassName("editable");
		comment.addFocusHandler(event -> haveFocus = comment);

		comment.addBlurHandler(event -> {
			haveFocus = null;

			if (m_currentShownPreset != null) {
				String oldInfo = m_currentShownPreset.comment;

				if (!oldInfo.equals(comment.getText())) {
					NonMaps.theMaps.getServerProxy().setPresetAttribute(m_currentShownPreset.uuid, "Comment",
							comment.getText());
				}
			}
		});

		name.addFocusHandler(event -> haveFocus = name);

		name.addBlurHandler(event -> {
			haveFocus = null;

			if (m_currentShownPreset != null) {
				String oldName = m_currentShownPreset.name;

				if (!oldName.equals(name.getText())) {
					NonMaps.theMaps.getServerProxy().renamePreset(m_currentShownPreset.uuid, name.getText());
				}
			}
		});

		name.addKeyPressHandler(arg0 -> {
			if (arg0.getCharCode() == KeyCodes.KEY_ENTER) {
				name.setFocus(false);
				comment.setFocus(true);
			}
		});

		position.addFocusHandler(event -> haveFocus = position);

		position.addBlurHandler(event -> {
			haveFocus = null;

			if (m_currentShownPreset != null) {
				var oldNumber = m_currentShownPreset.rawNumber;
				Integer newPos = position.getValue();
				if (newPos != null) {
					if (!newPos.equals(oldNumber)) {
						var bank = BankPresenterProviders.get().getPresenter(m_currentShownPreset.bankUuid);

						int targetPos = newPos;
						targetPos = Math.max(targetPos, 1);
						targetPos = Math.min(targetPos, bank.presets.size());

						if (targetPos == bank.presets.size())
							NonMaps.theMaps.getServerProxy().movePresetBelow(m_currentShownPreset.uuid,
									bank.presets.get(bank.presets.size() - 1));
						else if (targetPos > oldNumber)
							NonMaps.theMaps.getServerProxy().movePresetBelow(m_currentShownPreset.uuid,
									bank.presets.get(targetPos - 1));
						else
							NonMaps.theMaps.getServerProxy().movePresetAbove(m_currentShownPreset.uuid,
									bank.presets.get(targetPos - 1));
					}
				}

				position.setText(m_currentShownPreset.paddedNumber);
			}
		});

		position.addKeyPressHandler(arg0 -> {
			if (arg0.getCharCode() == KeyCodes.KEY_ENTER) {
				position.setFocus(false);
				name.setFocus(true);
			}
		});
		this.panel = panel;

		var uuid = PresetManagerPresenterProvider.get().getPresenter().selectedPreset;
		var presenter = PresetPresenterProviders.get().getPresenter(uuid);
		updateInfo(presenter, false);
	}

	private void addRow(FlexTable panel, String name, Widget content) {
		int c = panel.getRowCount();
		panel.setWidget(c, 0, new Label(name));
		panel.setWidget(c, 1, content);
	}

	private void saveContent() {
		if (haveFocus != null && m_currentShownPreset != null) {
			haveFocus.setFocus(false);
		}
	}
}
