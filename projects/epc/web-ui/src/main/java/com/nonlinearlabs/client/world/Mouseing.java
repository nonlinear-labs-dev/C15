package com.nonlinearlabs.client.world;

import java.util.ArrayList;

import com.google.gwt.core.client.JavaScriptObject;
import com.google.gwt.dom.client.NativeEvent;
import com.google.gwt.event.dom.client.ContextMenuEvent;
import com.google.gwt.event.dom.client.KeyDownEvent;
import com.google.gwt.event.dom.client.MouseWheelEvent;
import com.google.gwt.user.client.Event;
import com.google.gwt.user.client.Window;
import com.google.gwt.user.client.ui.FocusWidget;
import com.google.gwt.user.client.ui.RootPanel;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.world.pointer.PointerState;
import com.nonlinearlabs.client.world.pointer.Touch;

public abstract class Mouseing {

	PointerEvent pointerDown = new PointerEvent("pointerdown");
	PointerEvent pointerUp = new PointerEvent("pointerup");
	PointerEvent pointerMove = new PointerEvent("pointermove");

	ArrayList<Touch> touches = new ArrayList<Touch>();

	public Mouseing() {
		Window.addResizeHandler(new NonMapsResizeHandler(this));
		RootPanel.get().addDomHandler(new NonMapsWheelHandler(), MouseWheelEvent.getType());
	}

	public abstract void invalidate(int flags);

	public abstract void captureFocus();

	private static native void capturePointer(JavaScriptObject canvas, int id) /*-{
																				canvas.setPointerCapture(id);
																				}-*/;

	private static native String getPointerType(JavaScriptObject e) /*-{
																	return e.pointerType;
																	}-*/;

	private static native NativeEvent clone(JavaScriptObject event) /*-{
																	return new event.constructor(event.type, event);																	
																	}-*/;

	public void initHandlers(FocusWidget widget) {

		widget.addDoubleClickHandler(new NonMapsMouseDoubleClickHandler());

		NonLinearWorld nw = NonMaps.get().getNonLinearWorld();

		widget.addDomHandler((event) -> {
			Position p = new Position(event.getNativeEvent());
			nw.setCtrlDown(event.isControlKeyDown());
			nw.setShiftDown(event.isShiftKeyDown());

			capturePointer(widget.getElement(), event.pointerId);
			touches.add(new Touch(event.pointerId, p));

			if (getPointerType(event.getNativeEvent()) == "touch") {
				PointerState.get().onTouchStart(touches);
			} else {
				if (event.getNativeEvent().getButton() == com.google.gwt.dom.client.NativeEvent.BUTTON_LEFT)
					PointerState.get().onLeftDown(p, event.isControlKeyDown());
				else if (event.getNativeEvent().getButton() == com.google.gwt.dom.client.NativeEvent.BUTTON_RIGHT)
					PointerState.get().onRightDown(p);
			}
			NonMaps.theMaps.captureFocus();
		}, pointerDown.eventType);

		widget.addDomHandler((event) -> {
			event.preventDefault();
			Position p = new Position(event.getNativeEvent());

			touches.forEach(t -> {
				if (t.id == event.pointerId)
					t.pos = p;
			});

			if (getPointerType(event.getNativeEvent()) == "touch") {
				PointerState.get().onTouchMove(touches);
			} else {
				PointerState.get().onMove(p, event.isShiftKeyDown());
			}
		}, pointerMove.eventType);

		widget.addDomHandler((event) -> {
			Position p = new Position(event.getNativeEvent());

			touches.removeIf(t -> {
				return t.id == event.pointerId;
			});

			if (getPointerType(event.getNativeEvent()) == "touch") {
				PointerState.get().onTouchEnd(touches);
			} else {
				if (event.getNativeEvent().getButton() == com.google.gwt.dom.client.NativeEvent.BUTTON_LEFT)
					PointerState.get().onLeftUp(p);
				else if (event.getNativeEvent().getButton() == com.google.gwt.dom.client.NativeEvent.BUTTON_RIGHT)
					PointerState.get().onRightUp(p);
			}
		}, pointerUp.eventType);

		widget.addHandler(e -> {
			e.preventDefault();
			e.stopPropagation();
		}, ContextMenuEvent.getType());

		widget.sinkEvents(Event.ONCONTEXTMENU | Event.KEYEVENTS | Event.MOUSEEVENTS);
		widget.addKeyDownHandler(e -> NonMaps.get().getNonLinearWorld().handleKeyPress(e));
		widget.addKeyUpHandler(e -> NonMaps.get().getNonLinearWorld().handleKeyUp(e));
		widget.setFocus(true);
	}

	protected abstract boolean handleKeyPress(KeyDownEvent event);

}