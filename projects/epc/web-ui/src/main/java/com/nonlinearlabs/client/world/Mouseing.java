package com.nonlinearlabs.client.world;

import java.util.ArrayList;

import com.google.gwt.canvas.client.Canvas;
import com.google.gwt.core.client.JavaScriptObject;
import com.google.gwt.dom.client.Document;
import com.google.gwt.dom.client.Element;
import com.google.gwt.dom.client.NativeEvent;
import com.google.gwt.event.dom.client.ContextMenuEvent;
import com.google.gwt.event.dom.client.ContextMenuHandler;
import com.google.gwt.event.dom.client.KeyDownEvent;
import com.google.gwt.event.dom.client.KeyDownHandler;
import com.google.gwt.event.dom.client.KeyUpEvent;
import com.google.gwt.event.dom.client.KeyUpHandler;
import com.google.gwt.event.dom.client.MouseWheelEvent;
import com.google.gwt.user.client.Event;
import com.google.gwt.user.client.Window;
import com.google.gwt.user.client.ui.RootPanel;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.world.PointerEvent.GwtPointerEvent;
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

	private static native boolean isEventInside(JavaScriptObject element, JavaScriptObject event) /*-{
																									var rect =  element.getBoundingClientRect();
																									if(rect.left < event.clientX && rect.right > event.clientX)
																										if(rect.top < event.clientY && rect.bottom > event.clientY)
																											return true;
																									
																									return false;
																									}-*/;

	private boolean bubble(Element pm, Element element, GwtPointerEvent event) {
		for (var c = element.getLastChild(); c != null; c = c.getPreviousSibling()) {
			var child = (Element) c;
			var isEmpty = child.getClientHeight() == 0 || child.getClientWidth() == 0;
			if (isEmpty || isEventInside(child, event.getNativeEvent())) {
				if (bubble(pm, child, event))
					return true;
			}
		}

		if (element.getClientHeight() == 0 || element.getClientWidth() == 0) {
			return false;
		}

		if (pm == element)
			return false;

		element.dispatchEvent(clone(event.getNativeEvent()));
		return true;
	}

	public void initHandlers(Canvas canvas) {

		canvas.addDoubleClickHandler(new NonMapsMousDoubleClickHandler());

		NonLinearWorld nw = NonMaps.get().getNonLinearWorld();

		Event.addNativePreviewHandler((e) -> {

		});

		canvas.addDomHandler((event) -> {
			var pm = Document.get().getElementById("preset-manager");
			var orig = event.getRelativeElement();
			event.setRelativeElement(pm);
			if (bubble(pm, pm, event))
				return;

			event.setRelativeElement(orig);
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

		canvas.sinkEvents(Event.ONCONTEXTMENU | Event.KEYEVENTS | Event.MOUSEEVENTS);
		canvas.addKeyDownHandler(keypress);
		canvas.addKeyUpHandler(keyUpHandler);
		canvas.setFocus(true);
	}

	protected abstract boolean handleKeyPress(KeyDownEvent event);

}