package com.nonlinearlabs.NonMaps.client.world.overlay;

import java.util.Date;

import com.google.gwt.event.dom.client.BlurEvent;
import com.google.gwt.event.dom.client.BlurHandler;
import com.google.gwt.event.dom.client.ClickEvent;
import com.google.gwt.event.dom.client.ClickHandler;
import com.google.gwt.event.dom.client.FocusEvent;
import com.google.gwt.event.dom.client.FocusHandler;
import com.google.gwt.event.dom.client.KeyCodes;
import com.google.gwt.event.dom.client.KeyPressEvent;
import com.google.gwt.event.dom.client.KeyPressHandler;
import com.google.gwt.event.dom.client.MouseDownEvent;
import com.google.gwt.event.dom.client.MouseDownHandler;
import com.google.gwt.event.shared.HandlerRegistration;
import com.google.gwt.i18n.client.DateTimeFormat;
import com.google.gwt.i18n.client.DateTimeFormat.PredefinedFormat;
import com.google.gwt.i18n.shared.TimeZone;
import com.google.gwt.user.client.Event;
import com.google.gwt.user.client.Event.NativePreviewEvent;
import com.google.gwt.user.client.Event.NativePreviewHandler;
import com.google.gwt.user.client.ui.Button;
import com.google.gwt.user.client.ui.FlexTable;
import com.google.gwt.user.client.ui.HTMLPanel;
import com.google.gwt.user.client.ui.IntegerBox;
import com.google.gwt.user.client.ui.Label;
import com.google.gwt.user.client.ui.RootPanel;
import com.google.gwt.user.client.ui.TextArea;
import com.google.gwt.user.client.ui.TextBox;
import com.google.gwt.user.client.ui.Widget;
import com.nonlinearlabs.NonMaps.client.GMTTimeZone;
import com.nonlinearlabs.NonMaps.client.NonMaps;
import com.nonlinearlabs.NonMaps.client.world.maps.presets.bank.Bank;
import com.nonlinearlabs.NonMaps.client.world.maps.presets.bank.preset.ColorTag;
import com.nonlinearlabs.NonMaps.client.world.maps.presets.bank.preset.ColorTag.Color;
import com.nonlinearlabs.NonMaps.client.world.maps.presets.bank.preset.Preset;

public class PresetInfoDialog extends GWTDialog {

	private static PresetInfoDialog theDialog;
	private static float commentBoxHeight = 0;
	private TextArea comment;
	private Label deviceName;
	private Label softwareVersion;
	private Label storeTime;
	private Label bankName;
	private Widget haveFocus = null;
	private Preset thePreset;
	private Preset theEditPreset;
	private TextBox name;
	private Button[] colors;
	private IntegerBox position;
	
	
	private Button addColorButton(ColorTag.Color c) {		
		Button b = new Button();
		if(c == ColorTag.Color.none)
			b.setHTML("<div style=\"font-size: smaller;\">✕</div>"); 
		b.getElement().addClassName("colortaggingfield-" + c.name());
		b.getElement().addClassName("colortaggingfield-template");
		
		if(getCurrentPreset() != null) {
			updateCurrentHighlight(b);
		}
		
		b.addClickHandler(new ClickHandler() {
			@Override
			public void onClick(ClickEvent event) {
				NonMaps.get().getServerProxy().setPresetAttribute(getCurrentPreset(), "color", c.toString());
				updateCurrentHighlight(b);
			}
		});
		return b;
	}
	
	public void updateCurrentHighlight(Button current) {
		for(Button b: colors) {
			if(b != current)
				b.getElement().removeClassName("colortag-current");
			else
				b.getElement().addClassName("colortag-current");
		}
	}
	
	public Preset getCurrentPreset() {
		return thePreset;
	}
	
	private PresetInfoDialog() {
		RootPanel.get().add(this);

		getElement().addClassName("preset-info-dialog");

		initalShow();

		setAnimationEnabled(true);
		setGlassEnabled(false);
		setModal(false);

		colors = new Button[7];
		
		addHeader("Preset Info");
		addContent();
		
		initialSetup();
		
		super.pushDialogToFront();

		if (commentBoxHeight != 0) {
			comment.setHeight(commentBoxHeight + "px");
		}
	}

	private void initialSetup() {
		updateInfo(NonMaps.theMaps.getNonLinearWorld().getPresetManager().getSelectedPreset());
	}

	private void addRow(FlexTable panel, String name, Widget content) {
		int c = panel.getRowCount();
		panel.setWidget(c, 0, new Label(name));
		panel.setWidget(c, 1, content);
	}

	private void addContent() {

		HTMLPanel presetNameAndPositionBox = new HTMLPanel("div", "");
		presetNameAndPositionBox.getElement().addClassName("preset-name-and-pos");
		presetNameAndPositionBox.add(position = new IntegerBox());
		presetNameAndPositionBox.add(name = new TextBox());
		position.getElement().addClassName("position-box");
		name.getElement().addClassName("preset-name-box");

		HTMLPanel presetColorBox = new HTMLPanel("div", "");
		presetColorBox.getElement().addClassName("tag-div");
		presetColorBox.add(colors[Color.green.ordinal()] = addColorButton(Color.green));
		presetColorBox.add(colors[Color.blue.ordinal()] = addColorButton(Color.blue));
		presetColorBox.add(colors[Color.yellow.ordinal()] = addColorButton(Color.yellow));
		presetColorBox.add(colors[Color.orange.ordinal()] = addColorButton(Color.orange));
		presetColorBox.add(colors[Color.purple.ordinal()] = addColorButton(Color.purple));
		presetColorBox.add(colors[Color.red.ordinal()] = addColorButton(Color.red));
		presetColorBox.add(colors[Color.none.ordinal()] = addColorButton(Color.none));

		
		
		FlexTable panel = new FlexTable();
		addRow(panel, "Bank", bankName = new Label());
		addRow(panel, "Position/Name", presetNameAndPositionBox);
		addRow(panel, "Comment", comment = new TextArea());
		addRow(panel, "Color Tag", presetColorBox);
		addRow(panel, "Last Change", storeTime = new Label(""));
		addRow(panel, "Device Name", deviceName = new Label(""));
		addRow(panel, "UI Version", softwareVersion = new Label(""));

		position.getElement().addClassName("gwt-TextBox");

		comment.addFocusHandler(new FocusHandler() {

			@Override
			public void onFocus(FocusEvent event) {
				haveFocus = comment;
				theEditPreset = thePreset;
			}
		});

		comment.addBlurHandler(new BlurHandler() {

			@Override
			public void onBlur(BlurEvent event) {
				haveFocus = null;

				if (theEditPreset != null) {
					String oldInfo = theEditPreset.getAttribute("Comment");

					if (!oldInfo.equals(comment.getText())) {
						NonMaps.theMaps.getServerProxy().setPresetAttribute(theEditPreset, "Comment", comment.getText());
					}
				}
			}
		});

		comment.addMouseDownHandler(new MouseDownHandler() {
			private HandlerRegistration mouseMoveUpRegistration;
			private int lastWidth;
			private int lastHeight;

			@Override
			public void onMouseDown(MouseDownEvent event) {
				lastWidth = getOffsetWidth();
				lastHeight = getOffsetHeight();

				if (mouseMoveUpRegistration == null) {
					mouseMoveUpRegistration = Event.addNativePreviewHandler(new NativePreviewHandler() {
						@Override
						public void onPreviewNativeEvent(NativePreviewEvent event) {
							if (event.getTypeInt() == Event.ONMOUSEMOVE || event.getTypeInt() == Event.ONMOUSEUP) {
								int width = getOffsetWidth();
								int height = getOffsetHeight();
								if (width != lastWidth || height != lastHeight) {
									commentBoxHeight = comment.getElement().getClientHeight();

									lastWidth = width;
									lastHeight = height;
								}

								if (event.getTypeInt() == Event.ONMOUSEUP) {
									commentBoxHeight = comment.getElement().getClientHeight();

									if (mouseMoveUpRegistration != null) {
										mouseMoveUpRegistration.removeHandler();
										mouseMoveUpRegistration = null;
									}
								}
							}
						}
					});
				}
			}
		});

		name.addFocusHandler(new FocusHandler() {

			@Override
			public void onFocus(FocusEvent event) {
				haveFocus = name;
				theEditPreset = thePreset;
			}
		});

		name.addBlurHandler(new BlurHandler() {

			@Override
			public void onBlur(BlurEvent event) {
				haveFocus = null;

				if (theEditPreset != null) {
					String oldName = theEditPreset.getCurrentName();

					if (!oldName.equals(name.getText())) {
						NonMaps.theMaps.getServerProxy().renamePreset(theEditPreset.getUUID(), name.getText());
					}
				}
			}
		});

		name.addKeyPressHandler(new KeyPressHandler() {

			@Override
			public void onKeyPress(KeyPressEvent arg0) {
				if (arg0.getCharCode() == KeyCodes.KEY_ENTER) {
					name.setFocus(false);
					comment.setFocus(true);
				}
			}
		});

		position.addFocusHandler(new FocusHandler() {

			@Override
			public void onFocus(FocusEvent event) {
				haveFocus = position;
				theEditPreset = thePreset;
			}
		});

		position.addBlurHandler(new BlurHandler() {

			@Override
			public void onBlur(BlurEvent event) {
				haveFocus = null;

				if (theEditPreset != null) {
					int oldNumber = theEditPreset.getNumber();
					Integer newPos = position.getValue();
					if (newPos != null) {
						if (!newPos.equals(oldNumber)) {
							Bank bank = theEditPreset.getParent();
							int presetCount = bank.getPresetList().getPresetCount();
							int targetPos = newPos.intValue();
							targetPos = Math.max(targetPos, 1);
							targetPos = Math.min(targetPos, presetCount);

							if (targetPos == presetCount)
								NonMaps.theMaps.getServerProxy().movePresetBelow(theEditPreset, bank.getLast());
							else if (targetPos > oldNumber)
								NonMaps.theMaps.getServerProxy().movePresetBelow(theEditPreset, bank.getPreset(targetPos - 1));
							else
								NonMaps.theMaps.getServerProxy().movePresetAbove(theEditPreset, bank.getPreset(targetPos - 1));
						}
					}
				}

				position.setText(theEditPreset.getPaddedNumber());
			}
		});

		position.addKeyPressHandler(new KeyPressHandler() {

			@Override
			public void onKeyPress(KeyPressEvent arg0) {
				if (arg0.getCharCode() == KeyCodes.KEY_ENTER) {
					position.setFocus(false);
					name.setFocus(true);
				}
			}
		});

		setWidget(panel);
	}

	@Override
	protected void commit() {
		hide();
		theDialog = null;
		NonMaps.theMaps.captureFocus();
		NonMaps.theMaps.getNonLinearWorld().requestLayout();
	}

	public static void toggle() {
		if (theDialog != null) {
			theDialog.commit();
		} else {
			if (!NonMaps.theMaps.getNonLinearWorld().getPresetManager().isEmpty())
				theDialog = new PresetInfoDialog();
		}
	}

	public static boolean isShown() {
		return theDialog != null;
	}

	public static void update(Preset preset) {
		if (theDialog != null)
			theDialog.updateInfo(preset);
	}

	private void updateInfo(Preset preset) {
		thePreset = preset;

		if (preset != null) {
			String presetName = preset.getCurrentName();
			deviceName.setText(preset.getAttribute("DeviceName"));
			softwareVersion.setText(preset.getAttribute("SoftwareVersion"));
			storeTime.setText(localizeTime(preset.getAttribute("StoreTime")));
			String commentText = preset.getAttribute("Comment");

			if (haveFocus != comment) {
				if (!commentText.equals(comment.getText())) {
					comment.setText(commentText);
				}
			}

			if (haveFocus != name) {
				if (!presetName.equals(name.getText())) {
					name.setText(presetName);
				}
			}

			if (haveFocus != position) {
				position.setText(preset.getPaddedNumber());
			}

			Bank bank = preset.getParent();
			bankName.setText(bank.getOrderNumber() + " - " + bank.getTitleName());
			
			ColorTag.Color c = ColorTag.Color.toEnum(preset.getAttribute("color"));
			updateCurrentHighlight(colors[c.ordinal()]);
			
			centerIfOutOfView();
		}
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

	static int lastPopupLeft = -1;
	static int lastPopupTop = -1;

	@Override
	protected void setLastPopupPos(int popupLeft, int popupTop) {
		lastPopupLeft = popupLeft;
		lastPopupTop = popupTop;
	}

	@Override
	protected int getLastPopupPosTop() {
		return lastPopupTop;
	}

	@Override
	protected int getLastPopupPosLeft() {
		return lastPopupLeft;
	}

	public static void update() {
		update(NonMaps.theMaps.getNonLinearWorld().getPresetManager().getSelectedPreset());
	}
}