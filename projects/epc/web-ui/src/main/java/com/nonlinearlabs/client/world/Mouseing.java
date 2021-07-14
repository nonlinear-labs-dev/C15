package com.nonlinearlabs.client.world;

import java.util.ArrayList;

import com.google.gwt.canvas.client.Canvas;
import com.google.gwt.core.client.JavaScriptObject;
import com.google.gwt.event.dom.client.ContextMenuEvent;
import com.google.gwt.event.dom.client.ContextMenuHandler;
import com.google.gwt.event.dom.client.KeyDownEvent;
import com.google.gwt.event.dom.client.KeyDownHandler;
import com.google.gwt.event.dom.client.KeyUpEvent;
import com.google.gwt.event.dom.client.KeyUpHandler;
import com.google.gwt.user.client.Event;
import com.google.gwt.user.client.Window;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.world.PointerEvent.GwtPointerEvent;
import com.nonlinearlabs.client.world.pointer.PointerState;
import com.nonlinearlabs.client.world.pointer.Touch;

public abstract class Mouseing {

	PointerEvent pointerDown = new PointerEvent("pointerdown");
	PointerEvent pointerUp = new PointerEvent("pointerup");
	PointerEvent pointerMove = new PointerEvent("pointermove");
	PointerEvent pointerCancel = new PointerEvent("pointercancel");
	PointerEvent pointerLost = new PointerEvent("lostpointercapture");

	ArrayList<Touch> touches = new ArrayList<Touch>();

	public Mouseing() {
	}

	public abstract void invalidate(int flags);

	public abstract void captureFocus();

	private static native void capturePointer(JavaScriptObject canvas, int id) /*-{
																				canvas.setPointerCapture(id);
																				}-*/;

	private static native String getPointerType(JavaScriptObject e) /*-{
																	return e.pointerType;
																	}-*/;

	public void initHandlers(Canvas canvas) {
		Window.addResizeHandler(new NonMapsResizeHandler(this));
		canvas.addMouseWheelHandler(new NonMapsWheelHandler());
		canvas.addDoubleClickHandler(new NonMapsMousDoubleClickHandler());

		NonLinearWorld nw = NonMaps.get().getNonLinearWorld();

		canvas.addDomHandler((event) -> {
			Position p = new Position(event.getNativeEvent());

			nw.setCtrlDown(event.isControlKeyDown());
			nw.setShiftDown(event.isShiftKeyDown());

			capturePointer(canvas.getCanvasElement(), event.pointerId);
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

		canvas.addDomHandler((event) -> {
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

		canvas.addDomHandler((event) -> {
			onTouchEnd(event);
		}, pointerUp.eventType);

		canvas.addDomHandler((event) -> {
			onTouchEnd(event);
		}, pointerLost.eventType);

		canvas.addDomHandler((event) -> {
			onTouchEnd(event);
		}, pointerCancel.eventType);

		canvas.addHandler(new ContextMenuHandler() {
			@Override
			public void onContextMenu(ContextMenuEvent event) {
				event.preventDefault();
				event.stopPropagation();
			}
		}, ContextMenuEvent.getType());

		KeyDownHandler keypress = new KeyDownHandler() {
			@Override
			public void onKeyDown(KeyDownEvent event) {
				NonMaps.get().getNonLinearWorld().handleKeyPress(event);
			}
		};

		KeyUpHandler keyUpHandler = new KeyUpHandler() {
			@Override
			public void onKeyUp(KeyUpEvent event) {
				NonMaps.get().getNonLinearWorld().handleKeyUp(event);

			}
		};

		canvas.sinkEvents(Event.ONCONTEXTMENU | Event.KEYEVENTS | Event.MOUSEEVENTS | Event.TOUCHEVENTS);
		canvas.addKeyDownHandler(keypress);
		canvas.addKeyUpHandler(keyUpHandler);
		canvas.setFocus(true);
	}

	private void onTouchEnd(GwtPointerEvent event) {
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
	}

	protected abstract boolean handleKeyPress(KeyDownEvent event);

}