package com.nonlinearlabs.client.world.overlay;

import com.google.gwt.core.client.GWT;
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
import com.google.gwt.user.client.ui.FocusWidget;
import com.google.gwt.user.client.ui.HTMLPanel;
import com.google.gwt.user.client.ui.Label;
import com.google.gwt.user.client.ui.RootPanel;
import com.google.gwt.user.client.ui.TextArea;
import com.google.gwt.user.client.ui.TextBox;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.dataModel.editBuffer.ParameterId;
import com.nonlinearlabs.client.presenters.EditBufferPresenterProvider;
import com.nonlinearlabs.client.presenters.ParameterPresenter;
import com.nonlinearlabs.client.useCases.EditBufferUseCases;
import com.nonlinearlabs.client.world.Control;

public class ParameterInfoDialog extends GWTDialog {

	private static ParameterInfoDialog theDialog;
	private static float infoBoxHeight = 0;

	ParameterPresenter presenter;

	private TextArea infoField;
	private Label parameterNameView;
	private Label paramNameEditView;
	private TextBox paramNameEditEditor;
	private HTMLPanel paramNameEditor;
	private FocusWidget focusOwner;

	private ParameterInfoDialog() {
		RootPanel.get().add(this);

		getElement().addClassName("parameter-info-dialog");

		initalShow();

		setAnimationEnabled(true);
		setGlassEnabled(false);
		setModal(false);

		addHeader("Parameter Info");
		addContent();

		EditBufferPresenterProvider.get().onChange(p -> {
			update(p.selectedParameter);
			return true;
		});

		super.pushDialogToFront();

		GWT.log("ParameterInfo soll : " + infoBoxHeight);

		if (infoBoxHeight != 0) {
			infoField.setHeight(infoBoxHeight + "px");
		}
	}

	private void addContent() {
		FlexTable justToAddABorder = new FlexTable();
		HTMLPanel panel = new HTMLPanel("");
		panel.getElement().addClassName("vertical-panel");

		panel.add(parameterNameView = new Label());
		parameterNameView.getElement().addClassName("centered-text");

		paramNameEditor = new HTMLPanel("div", "");
		paramNameEditor.getElement().addClassName("flex-div");
		paramNameEditor.add(paramNameEditView = new Label());
		paramNameEditor.add(paramNameEditEditor = new TextBox());
		paramNameEditEditor.getElement().addClassName("param-name-edit editable");
		panel.add(paramNameEditor);

		HTMLPanel infoFieldBox = new HTMLPanel("div", "");
		infoFieldBox.getElement().addClassName("flex-div");
		infoFieldBox.add(infoField = new TextArea());
		infoField.getElement().addClassName("full-width");
		panel.add(infoFieldBox);

		infoField.addFocusHandler(new FocusHandler() {

			@Override
			public void onFocus(FocusEvent event) {
				focusOwner = infoField;
			}
		});

		infoField.addBlurHandler(new BlurHandler() {

			@Override
			public void onBlur(BlurEvent event) {
				focusOwner = null;
				EditBufferUseCases.get().setMacroControlInfo(presenter.id, infoField.getText());
				update(EditBufferPresenterProvider.getPresenter().selectedParameter);
			}
		});

		infoField.addMouseDownHandler(new MouseDownHandler() {
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
									infoBoxHeight = infoField.getElement().getClientHeight();

									lastWidth = width;
									lastHeight = height;
								}

								if (event.getTypeInt() == Event.ONMOUSEUP) {
									infoBoxHeight = infoField.getElement().getClientHeight();

									if (mouseMoveUpRegistration != null) {
										mouseMoveUpRegistration.removeHandler();
										mouseMoveUpRegistration = null;
									}
								}

								GWT.log("ParameterInfo resized to:" + infoBoxHeight);
							}
						}
					});
				}
			}
		});

		paramNameEditEditor.addKeyPressHandler(new KeyPressHandler() {

			@Override
			public void onKeyPress(KeyPressEvent arg0) {
				if (arg0.getCharCode() == KeyCodes.KEY_ENTER) {
					paramNameEditEditor.setFocus(false);
					infoField.setFocus(true);
				}
			}
		});

		paramNameEditEditor.addFocusHandler(new FocusHandler() {

			@Override
			public void onFocus(FocusEvent event) {
				focusOwner = paramNameEditEditor;
			}
		});

		paramNameEditEditor.addBlurHandler(new BlurHandler() {

			@Override
			public void onBlur(BlurEvent event) {
				focusOwner = null;
				ParameterId id = EditBufferPresenterProvider.getPresenter().selectedParameter.id;
				EditBufferUseCases.get().renameMacroControl(id, paramNameEditEditor.getText());
			}
		});

		justToAddABorder.setWidget(0, 0, panel);
		setWidget(justToAddABorder);
	}

	protected void update(ParameterPresenter selectedParameter) {
		if (focusOwner != null) {
			return;
		}

		presenter = EditBufferPresenterProvider.getPresenter().selectedParameter;
		boolean isMC = presenter.isMacroControl;

		infoField.setEnabled(isMC);
		paramNameEditor.setVisible(isMC);
		parameterNameView.setVisible(!isMC);

		if (isMC) {
			infoField.getElement().addClassName("editable");
			infoField.getElement().removeClassName("txt-area");
			paramNameEditView.getElement().addClassName("selectable-text");
		} else {
			infoField.getElement().removeClassName("editable");
			infoField.getElement().addClassName("txt-area");
			paramNameEditView.getElement().removeClassName("selectable-text");
		}

		if (isMC) {
			paramNameEditView.setText(presenter.fullNameWithGroup);

			if (focusOwner != paramNameEditEditor)
				paramNameEditEditor.setText(presenter.userGivenName);

			String info = presenter.parameterInfo;
			if (!info.isEmpty()) {
				setDescription(info);
				return;
			}
		}

		parameterNameView.setText(selectedParameter.fullNameWithGroup);
		String info = presenter.parameterInfo;
		if (!info.isEmpty()) {
			setDescription(info);
		}
		centerIfOutOfView();
	}

	@Override
	public String getTitle() {
		// Parameter p = param;

		// if (p != null) {
		// MapsControl parent = p.getParent();

		// while (parent != null) {
		// if (parent instanceof ParameterGroupIface) {
		// ParameterGroupIface group = (ParameterGroupIface) parent;
		// String longText = group.getName().getLongName() + " \u2013 " +
		// p.getName().getLongName();
		// return longText;
		// }
		// parent = parent.getParent();
		// }
		// }
		return "";
	}

	@Override
	protected void commit() {
		hide();
		NonMaps.theMaps.captureFocus();
		theDialog = null;
		NonMaps.theMaps.getNonLinearWorld().invalidate(Control.INVALIDATION_FLAG_UI_CHANGED);
	}

	public static void toggle() {
		if (theDialog != null) {
			theDialog.commit();
		} else {
			if (!NonMaps.theMaps.getNonLinearWorld().getPresetManager().isEmpty())
				theDialog = new ParameterInfoDialog();
		}
	}

	public static boolean isShown() {
		return theDialog != null;
	}

	public void setDescription(String text) {
		if (focusOwner != infoField)
			infoField.setText(text);

		infoField.setHeight("1em");
		int height = infoField.getElement().getScrollHeight() + 5;
		infoField.setHeight(Math.max(30, height) + "px");
	}

	public static void update() {
		theDialog.update(EditBufferPresenterProvider.getPresenter().selectedParameter);
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

	@Override
	protected void onAttach() {
		super.onAttach();
	}

	@Override
	protected void onDetach() {
		super.onDetach();
	}

}