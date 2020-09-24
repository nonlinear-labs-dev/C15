package com.nonlinearlabs.client.world.overlay.InfoDialog;

import java.util.Date;

import com.google.gwt.event.dom.client.KeyCodes;
import com.google.gwt.i18n.client.DateTimeFormat;
import com.google.gwt.i18n.client.DateTimeFormat.PredefinedFormat;
import com.google.gwt.user.client.ui.FlexTable;
import com.google.gwt.user.client.ui.FocusWidget;
import com.google.gwt.user.client.ui.HTMLPanel;
import com.google.gwt.user.client.ui.IntegerBox;
import com.google.gwt.user.client.ui.Label;
import com.google.gwt.user.client.ui.TextArea;
import com.google.gwt.user.client.ui.TextBox;
import com.google.gwt.user.client.ui.Widget;
import com.nonlinearlabs.client.GMTTimeZone;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.world.maps.presets.bank.Bank;
import com.nonlinearlabs.client.world.maps.presets.bank.preset.ColorTag;
import com.nonlinearlabs.client.world.maps.presets.bank.preset.ColorTag.Color;
import com.nonlinearlabs.client.world.maps.presets.bank.preset.Preset;

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
	private Preset m_currentShownPreset = null;

	private ColorTagBox colorBox = new ColorTagBox() {
		@Override
		public void onBoxClick(Color c) {
			NonMaps.get().getServerProxy().setPresetAttribute(PresetInfoWidget.get().m_currentShownPreset, "color",
					c.toString());
		}
	};

	public Widget panel;

	static public PresetInfoWidget get() {
		if (instance == null)
			instance = new PresetInfoWidget();
		return instance;
	}

	private String localizeTime(String iso) {
		try {
			DateTimeFormat f = DateTimeFormat.getFormat("yyyy-MM-ddTHH:mm:ssZZZZ");
			Date d = f.parse(iso);
			DateTimeFormat locale = DateTimeFormat.getFormat(PredefinedFormat.DATE_TIME_SHORT);
			return locale.format(d, new GMTTimeZone());
		} catch (Exception e) {
			return iso;
		}
	}

	public void updateInfo(Preset preset, boolean force) {
		if (m_currentShownPreset != preset) {
			saveContent();
			m_currentShownPreset = preset;
		}

		if (preset != null) {
			String presetName = preset.getCurrentName();
			deviceName.setText(preset.getAttribute("DeviceName"));
			softwareVersion.setText(preset.getAttribute("SoftwareVersion"));
			storeTime.setText(localizeTime(preset.getAttribute("StoreTime")));
			String commentText = preset.getAttribute("Comment");

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
				position.setText(preset.getPaddedNumber());
			}

			Bank bank = preset.getParent();
			bankName.setText(bank.getOrderNumber() + " - " + bank.getTitleName());
			colorBox.updateCurrentHighlight(ColorTag.Color.toEnum(preset.getAttribute("color")));
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
				String oldInfo = m_currentShownPreset.getAttribute("Comment");

				if (!oldInfo.equals(comment.getText())) {
					NonMaps.theMaps.getServerProxy().setPresetAttribute(m_currentShownPreset, "Comment",
							comment.getText());
				}
			}
		});

		name.addFocusHandler(event -> haveFocus = name);

		name.addBlurHandler(event -> {
			haveFocus = null;

			if (m_currentShownPreset != null) {
				String oldName = m_currentShownPreset.getCurrentName();

				if (!oldName.equals(name.getText())) {
					NonMaps.theMaps.getServerProxy().renamePreset(m_currentShownPreset.getUUID(), name.getText());
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
				int oldNumber = m_currentShownPreset.getNumber();
				Integer newPos = position.getValue();
				if (newPos != null) {
					if (!newPos.equals(oldNumber)) {
						Bank bank = m_currentShownPreset.getParent();
						int presetCount = bank.getPresetList().getPresetCount();
						int targetPos = newPos;
						targetPos = Math.max(targetPos, 1);
						targetPos = Math.min(targetPos, presetCount);

						if (targetPos == presetCount)
							NonMaps.theMaps.getServerProxy().movePresetBelow(m_currentShownPreset.getUUID(),
									bank.getLast().getUUID());
						else if (targetPos > oldNumber)
							NonMaps.theMaps.getServerProxy().movePresetBelow(m_currentShownPreset.getUUID(),
									bank.getPreset(targetPos - 1).getUUID());
						else
							NonMaps.theMaps.getServerProxy().movePresetAbove(m_currentShownPreset.getUUID(),
									bank.getPreset(targetPos - 1).getUUID());
					}
				}
			}

			if (m_currentShownPreset != null)
				position.setText(m_currentShownPreset.getPaddedNumber());
		});

		position.addKeyPressHandler(arg0 -> {
			if (arg0.getCharCode() == KeyCodes.KEY_ENTER) {
				position.setFocus(false);
				name.setFocus(true);
			}
		});
		this.panel = panel;

		updateInfo(NonMaps.get().getNonLinearWorld().getPresetManager().getSelectedPreset(), false);
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
