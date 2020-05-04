package com.nonlinearlabs.client;

import java.util.Date;
import java.util.LinkedList;

import com.google.gwt.core.client.JavaScriptObject;
import com.google.gwt.core.client.Scheduler;
import com.google.gwt.core.client.Scheduler.RepeatingCommand;
import com.google.gwt.event.dom.client.ClickEvent;
import com.google.gwt.event.dom.client.ClickHandler;
import com.google.gwt.user.client.Timer;
import com.google.gwt.user.client.Window;
import com.google.gwt.user.client.ui.Button;
import com.google.gwt.user.client.ui.HTMLPanel;
import com.google.gwt.user.client.ui.Label;
import com.google.gwt.user.client.ui.PopupPanel;
import com.nonlinearlabs.client.dataModel.setup.SetupModel;
import com.nonlinearlabs.client.world.overlay.GWTDialog;

class WebSocketConnection {
	JavaScriptObject webSocketConnection;
	private Timer pollTimer;
	private RepeatingCommand ping;
	private int pingCount = 0;
	private Date timeSent;
	private Timer notifyTimer;
	private int delay = 200;
	private LinkedList<String> pendingUpdates = new LinkedList<String>();

	private static class ConnectionErrorPopup extends PopupPanel {

		public ConnectionErrorPopup() {
			super(false);
			setWidget(new Label("The connection to the Nonlinear Instrument seems to be broken :-("));
			setGlassEnabled(true);
			center();
		}
	}

	private ConnectionErrorPopup connectionErrorPopup = null;

	private void showConnectionError() {
		Tracer.log("showConnectionError");
		if (connectionErrorPopup == null) {
			connectionErrorPopup = new ConnectionErrorPopup();
		}
	}

	private void hideConnectionError() {
		if (connectionErrorPopup != null) {
			connectionErrorPopup.hide();
			connectionErrorPopup = null;
		}
	}

	interface ServerListener {
		public void onServerConnectionOpened();

		public void onServerUpdate(String text);
	}

	ServerListener listener;

	public void startPolling(ServerListener listener) {
		Tracer.log("startPolling");
		this.listener = listener;
		if (Window.Location.getPort() == "8888") {
			webSocketOpen(Window.Location.getHostName() + ":8080");
		} else {
			webSocketOpen(Window.Location.getHost());
		}

		SetupModel.get().systemSettings.deviceName.onChange(t -> {
			if(lastDeviceName != null && !lastDeviceName.isEmpty() && !t.isEmpty() && lastDeviceName != t) {
				notifyHostChanged(lastDeviceName, t);
			}

			lastDeviceName = t;
			return true;
		});
	}

	private native int getBufferedAmount()
	/*-{
		return self.@com.nonlinearlabs.client.WebSocketConnection::webSocketConnection.bufferedAmount;
	}-*/;

	public native void webSocketOpen(String host)
	/*-{
		var address = 'ws://' + host + '/ws/';
		var connection = new WebSocket(address);
		var self = this;
	
		connection.onopen = function() {
			self.@com.nonlinearlabs.client.WebSocketConnection::webSocketConnection = connection;
			self.@com.nonlinearlabs.client.WebSocketConnection::onServerOpen(Ljava/lang/String;)('');
		};
	
		connection.onerror = function(error) {
			self.@com.nonlinearlabs.client.WebSocketConnection::onServerError(Ljava/lang/String;)('error');
		};
	
		connection.onclose = function(e) {
			self.@com.nonlinearlabs.client.WebSocketConnection::onServerClosed(Ljava/lang/String;)('closed');
		};
	
		connection.onmessage = function(e) {
			self.@com.nonlinearlabs.client.WebSocketConnection::onServerUpdate(Ljava/lang/String;)(e.data);
		};
	}-*/;

	public void send(String command) {
		doSend(command);
		timeSent = new Date();
	}

	private native void doSend(String command)
	/*-{
		if (this.@com.nonlinearlabs.client.WebSocketConnection::webSocketConnection)
			this.@com.nonlinearlabs.client.WebSocketConnection::webSocketConnection
					.send(command);
	}-*/;

	public void onServerUpdate(String data) {
		if (data.startsWith("/pong/")) {
			onPongReceived();
		} else {
			pendingUpdates.add(data);
			scheduleNotify();
		}
	}

	protected void scheduleNotify() {
		if (notifyTimer != null)
			notifyTimer.cancel();

		if (timeSent != null) {
			Date now = new Date();
			long diff = now.getTime() - timeSent.getTime();

			if (diff >= delay)
				notifyUpdates();
			else
				scheduleServerUpdate((int) (delay - diff));
		} else {
			notifyUpdates();
		}
	}

	private void notifyUpdates() {
		for (String s : pendingUpdates) {
			listener.onServerUpdate(s);
		}

		pendingUpdates.clear();
	}

	private void scheduleServerUpdate(int diff) {
		if (notifyTimer != null)
			notifyTimer.cancel();

		notifyTimer = new Timer() {

			@Override
			public void run() {
				scheduleNotify();
			}
		};

		notifyTimer.schedule(diff);
	}

	public void onServerError(String data) {
		reconnect(2000);
		showConnectionError();
	}

	public void onServerOpen(String data) {
		hideConnectionError();
		ping();
		listener.onServerConnectionOpened();
	}

	protected void ping() {
		if (ping == null) {
			send("/ping/" + pingCount);

			ping = new RepeatingCommand() {

				@Override
				public boolean execute() {
					if (ping == this) {
						showConnectionError();
						reconnect(1000);
					}
					return false;
				}
			};

			Scheduler.get().scheduleFixedPeriod(ping, 6000);
		}
	}

	public void onServerClosed(String data) {
		reconnect(2000);
		showConnectionError();
	}

	private void onPongReceived() {
		ping = null;
		hideConnectionError();
		Scheduler.get().scheduleFixedPeriod(new RepeatingCommand() {

			@Override
			public boolean execute() {
				ping();
				return false;
			}
		}, 2000);
	}

	private void reconnect(int timeout) {
		if (pollTimer == null) {
			pollTimer = new Timer() {
				@Override
				public void run() {
					pollTimer = null;
					startPolling(listener);
				}
			};

			pollTimer.schedule(timeout);
		}
	}

	static int ssid_left = 0;
	static int ssid_top = 0;
	static SSIDChangedDialog m_dialog = null;

	public class SSIDChangedDialog extends GWTDialog {

		public SSIDChangedDialog() {
			setModal(true);
			setWidth("20em");
			addHeader("C15 SSID change detected!");
	
			HTMLPanel panel = new HTMLPanel("");
			HTMLPanel buttons = new HTMLPanel("");
			panel.add(new Label("The SSID of the connected C15 instrument has changed. Do you want to reload the Graphical User Interface now?", true));
	
			Button yes, cancelButton;
	
			buttons.add(cancelButton = new Button("Cancel", new ClickHandler() {
	
				@Override
				public void onClick(ClickEvent arg0) {
					commit();
				}
			}));

			buttons.add(yes = new Button("Reload", new ClickHandler() {
	
				@Override
				public void onClick(ClickEvent arg0) {
					Window.Location.reload();
					commit();
				}
			}));

			yes.getElement().addClassName("modal-button-button");
			cancelButton.getElement().addClassName("modal-button-button");
			buttons.getElement().addClassName("modal-button-div");
			panel.add(buttons);
			add(panel);

			m_dialog = this;
		}

		@Override
		protected void setLastPopupPos(int popupLeft, int popupTop) {
			ssid_left = popupLeft;
			ssid_top = popupTop;
		}

		@Override
		protected int getLastPopupPosTop() {
			return ssid_top;
		}

		@Override
		protected int getLastPopupPosLeft() {
			return ssid_left;
		}

		@Override
		protected void commit() {
			hide();
			m_dialog = null;
		}

	};

	private void notifyHostChanged(String oldHost, String newHost) {
		SSIDChangedDialog dialog = new SSIDChangedDialog();
		dialog.center();
	}

	String lastDeviceName = null;

}