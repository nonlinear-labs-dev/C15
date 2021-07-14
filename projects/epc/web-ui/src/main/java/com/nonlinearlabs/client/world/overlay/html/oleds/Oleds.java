package com.nonlinearlabs.client.world.overlay.html.oleds;

import com.google.gwt.core.client.GWT;
import com.google.gwt.core.client.JavaScriptObject;
import com.google.gwt.uibinder.client.UiBinder;
import com.google.gwt.user.client.ui.Composite;
import com.google.gwt.user.client.ui.HTMLPanel;

public class Oleds extends Composite {
	interface SetupUiBinder extends UiBinder<HTMLPanel, Oleds> {
	}

	JavaScriptObject js = showOleds();

	private static SetupUiBinder ourUiBinder = GWT.create(SetupUiBinder.class);

	public Oleds() {
		initWidget(ourUiBinder.createAndBindUi(this));
	}

	@Override
	protected void onAttach() {
		super.onAttach();
		start(js);
	}

	@Override
	protected void onDetach() {
		super.onDetach();
		stop(js);
	}

	private static native JavaScriptObject showOleds() /*-{
        return new $wnd.showOleds();
    }-*/;

	private static native void start(JavaScriptObject o) /*-{
        o.start();
    }-*/;

	private static native void stop(JavaScriptObject o) /*-{
        o.stop();
    }-*/;
}
