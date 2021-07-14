package com.nonlinearlabs.client.world.overlay;

import com.google.gwt.event.dom.client.MouseDownEvent;
import com.google.gwt.event.dom.client.MouseDownHandler;
import com.google.gwt.event.dom.client.MouseUpEvent;
import com.google.gwt.event.dom.client.TouchEndEvent;
import com.google.gwt.event.dom.client.TouchEndHandler;
import com.google.gwt.event.dom.client.TouchMoveEvent;
import com.google.gwt.event.dom.client.TouchMoveHandler;
import com.google.gwt.event.dom.client.TouchStartEvent;
import com.google.gwt.event.dom.client.TouchStartHandler;
import com.google.gwt.event.logical.shared.CloseEvent;
import com.google.gwt.event.logical.shared.CloseHandler;
import com.google.gwt.user.client.Event;
import com.google.gwt.user.client.EventListener;
import com.google.gwt.user.client.Window;
import com.google.gwt.user.client.ui.Button;
import com.google.gwt.user.client.ui.DialogBox;
import com.google.gwt.user.client.ui.HTML;
import com.google.gwt.user.client.ui.HTMLPanel;
import com.google.gwt.user.client.ui.Label;
import com.google.gwt.user.client.ui.PopupPanel;
import com.google.gwt.user.client.ui.Widget;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.NonMaps.ScreenResizeListener;

public abstract class GWTDialog extends DialogBox implements ScreenResizeListener {

	static int maxZIndexSoFar = 1001;
	private Label headLabel;
	private Button close;

	public static class CaptionImpl extends HTML implements Caption {

		public CaptionImpl() {
			super();
			setStyleName("Caption");
		}
	}

	private class TouchHandler implements TouchStartHandler, TouchEndHandler, TouchMoveHandler {

		@Override
		public void onTouchMove(TouchMoveEvent event) {
			onMouseMove(getCaption().asWidget(), event.getTouches().get(0).getRelativeX(GWTDialog.this.getElement()),
					event.getTouches().get(0).getRelativeY(GWTDialog.this.getElement()));
		}

		@Override
		public void onTouchEnd(TouchEndEvent event) {
			onMouseUp(getCaption().asWidget(), 0, 0);
			centerIfOutOfView();

		}

		@Override
		public void onTouchStart(TouchStartEvent event) {
			int x = event.getTouches().get(0).getRelativeX(GWTDialog.this.getElement());
			int y = event.getTouches().get(0).getRelativeY(GWTDialog.this.getElement());
			if (y > getCaption().asWidget().getElement().getClientHeight())
				return;

			onMouseDown(getCaption().asWidget(), x, y);
		}
	}

	public GWTDialog() {
		super(new CaptionImpl());

		TouchHandler touchHandler = new TouchHandler();
		addDomHandler(touchHandler, TouchStartEvent.getType());
		addDomHandler(touchHandler, TouchMoveEvent.getType());
		addDomHandler(touchHandler, TouchEndEvent.getType());

		registerCloseHandler();
	}

	protected void pushDialogToFront() {
		getElement().getStyle().setZIndex(maxZIndexSoFar + 1);

		if (close != null)
			close.getElement().getStyle().setZIndex(maxZIndexSoFar + 2);

		maxZIndexSoFar++;
	}

	private void registerCloseHandler() {
		addCloseHandler(new CloseHandler<PopupPanel>() {

			@Override
			public void onClose(CloseEvent<PopupPanel> event) {
				setLastPopupPos(getPopupLeft(), getPopupTop());
			}
		});
	}

	protected void initalShow() {
		if (getLastPopupPosLeft() != -1 && getLastPopupPosTop() != -1)
			setPopupPosition(getLastPopupPosLeft(), getLastPopupPosTop());
		else
			center();

		show();
	}

	protected abstract void setLastPopupPos(int popupLeft, int popupTop);

	protected abstract int getLastPopupPosTop();

	protected abstract int getLastPopupPosLeft();

	protected void addHeader(String text) {
		close = new Button("");
		close.getElement().addClassName("headline-close");

		headLabel = new Label(text);
		headLabel.getElement().addClassName("headline");

		final HTMLPanel header = new HTMLPanel("");
		header.getElement().addClassName("dialog-header");
		header.add(headLabel);
		header.add(close);

		HTML captionHTML = (HTML) getCaption();
		captionHTML.getElement().appendChild(header.getElement());

		Event.sinkEvents(close.getElement(), Event.ONCLICK | Event.ONTOUCHSTART);
		Event.setEventListener(close.getElement(), new EventListener() {

			@Override
			public void onBrowserEvent(Event event) {
				if (event.getTypeInt() == Event.ONCLICK || event.getTypeInt() == Event.ONTOUCHSTART)
					commit();
			}
		});
	}

	protected void setHeaderText(String title) {
		headLabel.setText(title);
	}

	protected void centerIfOutOfView() {
		int left = getPopupLeft();
		int top = getPopupTop();
		int right = left + getOffsetWidth();
		int bottom = top + getOffsetHeight();
		int vpWidth = (int) (NonMaps.theMaps.getNonLinearWorld().getViewport().getPixRect().getWidth()
				/ NonMaps.devicePixelRatio);
		int vpHeight = (int) (NonMaps.theMaps.getNonLinearWorld().getViewport().getPixRect().getHeight()
				/ NonMaps.devicePixelRatio);
		boolean toMuchRight = (right >= vpWidth);
		boolean toMuchDown = (bottom >= vpHeight);
		boolean toMuchUp = (top <= 0);
		boolean toMuchLeft = (left <= 0);

		while (toMuchDown || toMuchRight || toMuchUp || toMuchLeft) {
			if (toMuchDown) {
				setPopupPosition(getPopupLeft(), Window.getClientHeight() - getOffsetHeight());
				toMuchDown = false;
			} else if (toMuchRight) {
				setPopupPosition(Window.getClientWidth() - getOffsetWidth(), getPopupTop());
				toMuchRight = false;
			} else if (toMuchUp) {
				setPopupPosition(getPopupLeft(), 0);
				toMuchUp = false;
			} else if (toMuchLeft) {
				setPopupPosition(0, getPopupTop());
				toMuchLeft = false;
			}
		}
	}

	protected abstract void commit();

	@Override
	public void onScreenResize(int width, int height) {
		centerIfOutOfView();
	}

	@Override
	protected void onAttach() {
		super.onAttach();
		NonMaps.theMaps.registerScreenResizeListener(this);
	}

	@Override
	protected void onDetach() {
		NonMaps.theMaps.unregisterScreenResizeListener(this);
		super.onDetach();
	}

	@Override
	protected void endDragging(MouseUpEvent event) {
		int marginW = getOffsetWidth() / 2;
		int marginH = 40;

		int leftMargin = -(getOffsetWidth() - marginW);
		int lowerMargin = Window.getClientHeight() - marginH;
		int rightMargin = Window.getClientWidth() - marginW;
		int upperMargin = 0;

		if (getAbsoluteLeft() > rightMargin)
			setPopupPosition(rightMargin, getPopupTop());
		if (getAbsoluteLeft() < leftMargin)
			setPopupPosition(leftMargin, getPopupTop());
		if (getAbsoluteTop() > lowerMargin)
			setPopupPosition(getPopupLeft(), lowerMargin);
		if (getAbsoluteTop() < upperMargin)
			setPopupPosition(getPopupLeft(), upperMargin);

		super.endDragging(event);
	}

	@Override
	public void setWidget(Widget w) {
		super.setWidget(w);
		addBringToFrontHandlers(w);
		addBringToFrontHandlers(getCaption().asWidget());
	}

	private void addBringToFrontHandlers(Widget w) {
		w.addDomHandler(new MouseDownHandler() {
			public void onMouseDown(MouseDownEvent event) {
				pushDialogToFront();
			}
		}, MouseDownEvent.getType());

		w.addDomHandler(new TouchStartHandler() {

			@Override
			public void onTouchStart(TouchStartEvent event) {
				pushDialogToFront();
			}
		}, TouchStartEvent.getType());

		w.sinkEvents(Event.MOUSEEVENTS | Event.TOUCHEVENTS);
	}

}