package com.nonlinearlabs.client.world.overlay;

import com.google.gwt.event.dom.client.BlurEvent;
import com.google.gwt.event.dom.client.BlurHandler;
import com.google.gwt.event.dom.client.FocusEvent;
import com.google.gwt.event.dom.client.FocusHandler;
import com.google.gwt.event.dom.client.KeyCodes;
import com.google.gwt.event.dom.client.KeyPressEvent;
import com.google.gwt.event.dom.client.KeyPressHandler;
import com.google.gwt.event.dom.client.MouseDownEvent;
import com.google.gwt.event.dom.client.MouseDownHandler;
import com.google.gwt.event.shared.HandlerRegistration;
import com.google.gwt.user.client.Event;
import com.google.gwt.user.client.Event.NativePreviewEvent;
import com.google.gwt.user.client.Event.NativePreviewHandler;
import com.google.gwt.user.client.ui.FlexTable;
import com.google.gwt.user.client.ui.HTMLPanel;
import com.google.gwt.user.client.ui.IntegerBox;
import com.google.gwt.user.client.ui.Label;
import com.google.gwt.user.client.ui.RootPanel;
import com.google.gwt.user.client.ui.TextArea;
import com.google.gwt.user.client.ui.TextBox;
import com.google.gwt.user.client.ui.Widget;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.presenters.BankPresenter;
import com.nonlinearlabs.client.presenters.BankPresenterProviders;
import com.nonlinearlabs.client.presenters.PresetManagerPresenterProvider;
import com.nonlinearlabs.client.useCases.BankUseCases;

public class BankInfoDialog extends GWTDialog {

	private static BankInfoDialog theDialog;

	private static float commentBoxHeight = 0;

	private TextBox name;
	private IntegerBox position;
	private TextArea comment;
	private Label size;
	private Label lastChange;
	private Label importFileDate;
	private Label importFileName;
	private Label stateLabel, exportFileName, exportFileDate;

	private BankPresenter bankPresenter;

	private Widget haveFocus = null;

	private BankInfoDialog() {
		RootPanel.get().add(this);

		getElement().addClassName("bank-info-dialog");

		initalShow();

		setAnimationEnabled(true);
		setGlassEnabled(false);
		setModal(false);

		addHeader("Bank Info");
		addContent();

		initialSetup();

		super.pushDialogToFront();

		if (commentBoxHeight != 0) {
			comment.setHeight(commentBoxHeight + "px");
		}
	}

	private void initialSetup() {
		var pmPresenter = PresetManagerPresenterProvider.get().getPresenter();
		var bankPresenter = BankPresenterProviders.get().getPresenter(pmPresenter.selectedBank);
		updateInfo(bankPresenter);
	}

	private void addRow(FlexTable panel, String name, Widget content) {
		int c = panel.getRowCount();
		panel.setWidget(c, 0, new Label(name));
		panel.setWidget(c, 1, content);
	}

	private void addContent() {
		HTMLPanel bankNameAndPositionBox = new HTMLPanel("div", "");
		bankNameAndPositionBox.getElement().addClassName("preset-name-and-pos");
		bankNameAndPositionBox.add(position = new IntegerBox());
		bankNameAndPositionBox.add(name = new TextBox());
		position.getElement().addClassName("position-box editable");
		name.getElement().addClassName("preset-name-box editable");

		position.setText("1");

		FlexTable panel = new FlexTable();
		addRow(panel, "Position/Name", bankNameAndPositionBox);
		addRow(panel, "Comment", comment = new TextArea());
		addRow(panel, "Size", size = new Label(""));
		addRow(panel, "State", stateLabel = new Label(""));
		addRow(panel, "Last Change", lastChange = new Label(""));
		addRow(panel, "Import Date", importFileDate = new Label(""));
		addRow(panel, "Import File", importFileName = new Label(""));
		addRow(panel, "Export Date", exportFileDate = new Label(""));
		addRow(panel, "Export File", exportFileName = new Label(""));

		position.getElement().addClassName("gwt-TextBox");

		comment.getElement().addClassName("editable");
		comment.addFocusHandler(new FocusHandler() {

			@Override
			public void onFocus(FocusEvent event) {
				setFocus(comment);
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

		comment.addBlurHandler(new BlurHandler() {

			@Override
			public void onBlur(BlurEvent event) {
				haveFocus = null;

				if (bankPresenter != null) {
					String oldInfo = bankPresenter.comment;

					if (!oldInfo.equals(comment.getText())) {
						BankUseCases.get().setComment(bankPresenter.uuid, comment.getText());
					}
				}
			}
		});

		position.addKeyPressHandler(new KeyPressHandler() {

			@Override
			public void onKeyPress(KeyPressEvent event) {
				if (event.getCharCode() == KeyCodes.KEY_ENTER) {
					position.setFocus(false);
					name.setFocus(true);
				}
			}
		});

		position.addFocusHandler(new FocusHandler() {

			@Override
			public void onFocus(FocusEvent event) {
				setFocus(position);
			}
		});

		position.addBlurHandler(new BlurHandler() {

			@Override
			public void onBlur(BlurEvent event) {
				haveFocus = null;

				if (bankPresenter != null) {
					int oldNumber = bankPresenter.orderNumber;
					int currentValue = position.getValue();

					if (oldNumber != currentValue) {
						BankUseCases.get().setOrderNumber(bankPresenter.uuid, currentValue);

					}
				}
			}
		});

		name.addFocusHandler(new FocusHandler() {

			@Override
			public void onFocus(FocusEvent event) {
				setFocus(name);
			}
		});

		name.addBlurHandler(new BlurHandler() {

			@Override
			public void onBlur(BlurEvent event) {
				haveFocus = null;

				if (bankPresenter != null) {
					String oldName = bankPresenter.name;

					if (!oldName.equals(name.getText())) {
						BankUseCases.get().rename(bankPresenter.uuid, name.getText());
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

		setWidget(panel);
		setFocus(panel);
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
			var presenter = PresetManagerPresenterProvider.get().getPresenter();
			if (!presenter.selectedBank.isEmpty())
				theDialog = new BankInfoDialog();
		}
	}

	public static boolean isShown() {
		return theDialog != null;
	}

	public static void update(BankPresenter bank) {
		if (theDialog != null)
			theDialog.updateInfo(bank);
	}

	private void updateInfo(BankPresenter bankPresenter) {
		if (bankPresenter != null) {
			String bankName = bankPresenter.name;
			String commentText = bankPresenter.comment;
			int bankPos = bankPresenter.orderNumber;

			if (haveFocus != comment) {
				if (!commentText.equals(comment.getText())) {
					comment.setText(commentText);

					if (comment.getElement().getScrollHeight() > 0) {
						comment.setHeight("1em");
						int height = comment.getElement().getScrollHeight() + 5;
						comment.setHeight(height + "px");
					}
				}
			}

			if (haveFocus != name) {
				if (!bankName.equals(name.getText())) {
					name.setText(bankName);
				}
			}

			Integer currentPositionValue = position.getValue();

			if (haveFocus != position) {
				if (bankPos != currentPositionValue) {
					position.setText(Integer.toString(bankPos));
				}
			}

			size.setText(Integer.toString(bankPresenter.presets.size()));

			try {
				lastChange.setText(bankPresenter.dateOfLastChange);
			} catch (Exception e) {
				lastChange.setText("---");
			}

			try {
				importFileDate.setText(bankPresenter.dateOfImportFile);
			} catch (Exception e) {
				importFileDate.setText("---");
			}

			try {
				exportFileDate.setText(bankPresenter.dateOfExportFile);
			} catch (Exception e) {
				exportFileDate.setText("---");
			}

			try {
				exportFileName.setText(bankPresenter.nameOfExportFile);
			} catch (Exception e) {
				exportFileName.setText("---");
			}

			try {
				stateLabel.setText(bankPresenter.importExportState);
			} catch (Exception e) {
				stateLabel.setText("---");
			}

			importFileName.setText(bankPresenter.nameOfImportFile);

			centerIfOutOfView();
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

	private void setFocus(Widget w) {
		haveFocus = w;
		var pm = PresetManagerPresenterProvider.get().getPresenter();
		bankPresenter = BankPresenterProviders.get().getPresenter(pm.selectedBank);
	}

	public static void update() {
		var pm = PresetManagerPresenterProvider.get().getPresenter();
		var bankPresenter = BankPresenterProviders.get().getPresenter(pm.selectedBank);
		update(bankPresenter);
	}
}