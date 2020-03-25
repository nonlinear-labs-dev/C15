package com.nonlinearlabs.client.world;

import com.google.gwt.core.client.JavaScriptObject;
import com.google.gwt.event.dom.client.DomEvent.Type;
import com.google.gwt.event.dom.client.HumanInputEvent;
import com.google.gwt.event.shared.EventHandler;

public class PointerEvent {

    public Type<Handler> eventType;

    public PointerEvent(String name) {
        eventType = new Type<Handler>(name, new Event());
    }

    public static abstract class GwtPointerEvent extends HumanInputEvent<Handler> {

        public int pointerId = 0;

        private native int getPointerId(JavaScriptObject e) /*-{
                                                            return e.pointerId;
                                                            }-*/;

        @Override
        protected void dispatch(Handler handler) {
            pointerId = getPointerId(getNativeEvent());
            handler.onEvent(this);
        }

    }

    public class Event extends GwtPointerEvent {

        @Override
        public Type<Handler> getAssociatedType() {
            return eventType;
        }

    }

    public interface Handler extends EventHandler {
        void onEvent(GwtPointerEvent event);
    }
}