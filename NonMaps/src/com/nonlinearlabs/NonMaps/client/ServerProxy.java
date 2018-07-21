package com.nonlinearlabs.NonMaps.client;

import java.util.List;

import com.google.gwt.core.client.JavaScriptObject;
import com.google.gwt.http.client.URL;
import com.google.gwt.user.client.Window;
import com.google.gwt.xhr.client.ReadyStateChangeHandler;
import com.google.gwt.xhr.client.XMLHttpRequest;
import com.google.gwt.xml.client.Document;
import com.google.gwt.xml.client.Element;
import com.google.gwt.xml.client.NamedNodeMap;
import com.google.gwt.xml.client.Node;
import com.google.gwt.xml.client.NodeList;
import com.google.gwt.xml.client.XMLParser;
import com.nonlinearlabs.NonMaps.client.WebSocketConnection.ServerListener;
import com.nonlinearlabs.NonMaps.client.contextStates.StopWatchState;
import com.nonlinearlabs.NonMaps.client.dataModel.DeviceInfoUpdater;
import com.nonlinearlabs.NonMaps.client.dataModel.SetupUpdater;
import com.nonlinearlabs.NonMaps.client.world.Control;
import com.nonlinearlabs.NonMaps.client.world.IBank;
import com.nonlinearlabs.NonMaps.client.world.IPreset;
import com.nonlinearlabs.NonMaps.client.world.RenameDialog;
import com.nonlinearlabs.NonMaps.client.world.Uuid;
import com.nonlinearlabs.NonMaps.client.world.maps.NonDimension;
import com.nonlinearlabs.NonMaps.client.world.maps.NonPosition;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.ModulatableParameter;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.Parameter;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.ParameterEditor;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.PlayControls.MacroControls.MacroControlParameter;
import com.nonlinearlabs.NonMaps.client.world.maps.presets.PresetManager.SearchQueryCombination;
import com.nonlinearlabs.NonMaps.client.world.maps.presets.bank.Bank;
import com.nonlinearlabs.NonMaps.client.world.maps.presets.bank.Tape.Orientation;
import com.nonlinearlabs.NonMaps.client.world.maps.presets.bank.preset.Preset;
import com.nonlinearlabs.NonMaps.client.world.overlay.ParameterInfoDialog;
import com.nonlinearlabs.NonMaps.client.world.overlay.PresetInfoDialog;

public class ServerProxy {

	private NonMaps nonMaps = null;
	private WebSocketConnection webSocket;
	private String nonmapsVersion = null;
	private String playgroundVersion = null;

	ServerProxy(NonMaps nonMaps) {
		this.nonMaps = nonMaps;
	}

	void startPolling() {
		webSocket = new WebSocketConnection();
		webSocket.startPolling(new ServerListener() {

			@Override
			public void onServerUpdate(String text) {
				applyChanges(text);
			}

			@Override
			public void onServerConnectionOpened() {
				if (nonmapsVersion == null) {
					downloadFile("nonmaps-version.txt", new DownloadHandler() {

						@Override
						public void onFileDownloaded(String text) {
							nonmapsVersion = text;
						}

						@Override
						public void onError() {
						}
					});
				}
			}
		});
	}

	private void queueJob(RemoteProcedureCall uri, boolean isOracle) {
		webSocket.send(uri.getURI(isOracle));
	}

	private void applyChanges(String responseText) {
		try (StopWatchState s = new StopWatchState("ServerProxy::applyChanges")) {
			Document xml = XMLParser.parse(responseText);
			Node world = xml.getElementsByTagName("nonlinear-world").item(0);
			Node editBufferNode = xml.getElementsByTagName("edit-buffer").item(0);
			Node settingsNode = xml.getElementsByTagName("settings").item(0);
			Node undoNode = xml.getElementsByTagName("undo").item(0);
			Node presetManagerNode = xml.getElementsByTagName("preset-manager").item(0);
			Node deviceInfo = xml.getElementsByTagName("device-information").item(0);
			Node clipboardInfo = xml.getElementsByTagName("clipboard").item(0);

			boolean omitOracles = omitOracles(world);
			Tracer.log("ServerProxy.applyChanges -> omitOracles = " + omitOracles);

			nonMaps.getNonLinearWorld().getClipboardManager().update(clipboardInfo);
			nonMaps.getNonLinearWorld().getParameterEditor().update(editBufferNode, omitOracles);
			nonMaps.getNonLinearWorld().getPresetManager().update(presetManagerNode);
			nonMaps.getNonLinearWorld().getViewport().getOverlay()
					.update(settingsNode, editBufferNode, presetManagerNode, deviceInfo, undoNode);
			nonMaps.getNonLinearWorld().invalidate(Control.INVALIDATION_FLAG_UI_CHANGED);
			
			if(PresetInfoDialog.isShown()) {
				PresetInfoDialog.theDialog.updateEditBuffer(editBufferNode);
			} else {
				PresetInfoDialog.setLastUpdateNode(editBufferNode);
			}

			setPlaygroundSoftwareVersion(deviceInfo);
			checkSoftwareVersionCompatibility();

			SetupUpdater setupUpdater = new SetupUpdater(settingsNode.getFirstChild());
			setupUpdater.doUpdate();

			DeviceInfoUpdater deviceInfoUpdater = new DeviceInfoUpdater(deviceInfo);
			deviceInfoUpdater.doUpdate();
		}
	}

	private void checkSoftwareVersionCompatibility() {
		if (playgroundVersion != null && !playgroundVersion.isEmpty() && nonmapsVersion != null && !nonmapsVersion.isEmpty()) {
			if (!playgroundVersion.equals(nonmapsVersion)) {
				boolean reload = Window.confirm("WebUI has to be reloaded. The C15 software version is " + this.playgroundVersion
						+ " while the WebUI version is " + nonmapsVersion + ".");

				if (reload) {
					nonmapsVersion = null;
					Window.Location.reload();
				}
			}
		}
	}

	private void setPlaygroundSoftwareVersion(Node deviceInfo) {
		String playgroundVersion = getChildText(deviceInfo, "software-version");
		if (playgroundVersion != null && !playgroundVersion.isEmpty()) {
			this.playgroundVersion = playgroundVersion;
		}
	}

	private boolean omitOracles(Node world) {
		try {
			return world.getAttributes().getNamedItem("omit-oracles").getNodeValue().equals("1");
		} catch (Exception e) {
			Tracer.log("ServerProxy.omitOracles -> exception");
		}
		Tracer.log("ServerProxy.omitOracles -> hard coded false");
		return false;
	}

	public void onParameterSelectionChanged(final ParameterEditor root) {
		Parameter pl = root.getSelection();
		if (pl != null) {
			StaticURI.Path path = new StaticURI.Path("presets", "param-editor", "select-param");
			StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("id", pl.getParameterID()));
			queueJob(uri, true);
		}
	}

	public void onParameterChanged(Parameter pl) {
		StaticURI.Path path = new StaticURI.Path("presets", "param-editor", "set-param");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("id", pl.getParameterID()), new StaticURI.KeyValue("value", pl
				.getValue().getQuantizedClipped()));
		queueJob(uri, pl.isOracle());
	}

	public void selectPreset(String uuid) {
		StaticURI.Path path = new StaticURI.Path("presets", "banks", "select-preset");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("uuid", uuid));
		queueJob(uri, false);
	}

	public void newBank(String initialName, NonPosition pos) {
		StaticURI.Path path = new StaticURI.Path("presets", "new-bank");
		StaticURI.KeyValue x = new StaticURI.KeyValue("x", pos.getX());
		StaticURI.KeyValue y = new StaticURI.KeyValue("y", pos.getY());
		StaticURI.KeyValue name = new StaticURI.KeyValue("name", initialName);
		StaticURI uri = new StaticURI(path, x, y, name);
		queueJob(uri, false);
	}
	
	public void newBankFromEditBuffer(NonPosition nonPosition) {
		StaticURI.Path path = new StaticURI.Path("presets", "new-bank-from-edit-buffer");
		StaticURI.KeyValue x = new StaticURI.KeyValue("x", nonPosition.getX());
		StaticURI.KeyValue y = new StaticURI.KeyValue("y", nonPosition.getY());
		StaticURI uri = new StaticURI(path, x, y);
		queueJob(uri, false);
	}

	public void renameBank(String uuid, String newName) {
		StaticURI.Path path = new StaticURI.Path("presets", "rename-bank");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("uuid", uuid), new StaticURI.KeyValue("name", newName));
		queueJob(uri, false);
	}

	public void selectBank(String uuid) {
		StaticURI.Path path = new StaticURI.Path("presets", "select-bank");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("uuid", uuid));
		queueJob(uri, false);
	}

	public void loadPreset(IPreset preset) {
		loadPreset(preset.getUUID());
	}

	public void loadPreset(String presetuuid) {
		StaticURI.Path path = new StaticURI.Path("presets", "banks", "load-preset");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("uuid", presetuuid));
		queueJob(uri, false);
	}

	public void createNewBankFromPreset(NonPosition nonPosition, IPreset p) {
		StaticURI.Path path = new StaticURI.Path("presets", "banks", "create-new-bank-from-preset");
		StaticURI.KeyValue x = new StaticURI.KeyValue("x", nonPosition.getX());
		StaticURI.KeyValue y = new StaticURI.KeyValue("y", nonPosition.getY());
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("preset", p.getUUID()), x, y);
		queueJob(uri, false);
	}

	public void createNewBankFromPresets(NonPosition nonPosition, String csv) {
		StaticURI.Path path = new StaticURI.Path("presets", "banks", "create-new-bank-from-presets");
		StaticURI.KeyValue x = new StaticURI.KeyValue("x", nonPosition.getX());
		StaticURI.KeyValue y = new StaticURI.KeyValue("y", nonPosition.getY());
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("presets", csv), x, y);
		queueJob(uri, false);
	}

	public void renamePreset(String uuid, String newName) {
		StaticURI.Path path = new StaticURI.Path("presets", "banks", "rename-preset");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("uuid", uuid), new StaticURI.KeyValue("name", newName));
		queueJob(uri, false);
	}

	public void onBankPositionChanged(Bank bank) {
		NonPosition bankPos = bank.getNonPosition().getPosition();
		StaticURI.Path path = new StaticURI.Path("presets", "banks", "set-position");
		StaticURI.KeyValue x = new StaticURI.KeyValue("x", bankPos.getX());
		StaticURI.KeyValue y = new StaticURI.KeyValue("y", bankPos.getY());
		StaticURI.KeyValue uuid = new StaticURI.KeyValue("uuid", bank.getUUID());
		StaticURI uri = new StaticURI(path, x, y, uuid);
		queueJob(uri, true);
	}

	public void setBankOrderNumber(Bank bank, int newOrderNumber) {
		StaticURI.Path path = new StaticURI.Path("presets", "banks", "set-order-number");
		StaticURI.KeyValue uuid = new StaticURI.KeyValue("uuid", bank.getUUID());
		StaticURI.KeyValue order = new StaticURI.KeyValue("order-number", newOrderNumber);
		StaticURI uri = new StaticURI(path, uuid, order);
		queueJob(uri, true);
	}

	public void dropPresetsAbove(String csv, IPreset actionAnchor) {
		StaticURI.Path path = new StaticURI.Path("presets", "banks", "drop-presets-above");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("presets", csv),
				new StaticURI.KeyValue("anchor", actionAnchor.getUUID()));
		queueJob(uri, false);

	}

	public void dropPresetsBelow(String csv, IPreset actionAnchor) {
		StaticURI.Path path = new StaticURI.Path("presets", "banks", "drop-presets-below");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("presets", csv),
				new StaticURI.KeyValue("anchor", actionAnchor.getUUID()));
		queueJob(uri, false);

	}

	public void dropPresetsTo(String csv, IPreset actionAnchor) {
		StaticURI.Path path = new StaticURI.Path("presets", "banks", "drop-presets-to");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("presets", csv),
				new StaticURI.KeyValue("anchor", actionAnchor.getUUID()));
		queueJob(uri, false);
	}

	public void movePresetAbove(IPreset p, IPreset actionAnchor) {
		StaticURI.Path path = new StaticURI.Path("presets", "banks", "move-preset-above");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("presetToMove", p.getUUID()), new StaticURI.KeyValue("anchor",
				actionAnchor.getUUID()));
		queueJob(uri, false);
	}

	public void movePresetBelow(IPreset p, IPreset actionAnchor) {
		StaticURI.Path path = new StaticURI.Path("presets", "banks", "move-preset-below");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("presetToMove", p.getUUID()), new StaticURI.KeyValue("anchor",
				actionAnchor.getUUID()));
		queueJob(uri, false);
	}

	public void overwritePresetWith(IPreset p, IPreset actionAnchor) {
		StaticURI.Path path = new StaticURI.Path("presets", "banks", "overwrite-preset");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("presetToOverwrite", actionAnchor.getUUID()), new StaticURI.KeyValue(
				"overwriteWith", p.getUUID()));
		queueJob(uri, false);
	}

	public void overwritePreset() {
		StaticURI.Path path = new StaticURI.Path("presets", "banks", "overwrite-preset");
		StaticURI uri = new StaticURI(path);
		queueJob(uri, false);
		RenameDialog.awaitNewPreset(NonMaps.get().getNonLinearWorld().getPresetManager().getSelectedPresetUUID());
	}

	public void movePresetTo(IPreset p, IPreset actionAnchor) {
		StaticURI.Path path = new StaticURI.Path("presets", "banks", "move-preset-to");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("presetToOverwrite", actionAnchor.getUUID()), new StaticURI.KeyValue(
				"overwriteWith", p.getUUID()));
		queueJob(uri, false);
	}

	public void insertPresetCopyAbove(IPreset p, IPreset preset) {
		StaticURI.Path path = new StaticURI.Path("presets", "banks", "copy-preset-above");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("presetToCopy", p.getUUID()), new StaticURI.KeyValue("anchor",
				preset.getUUID()));
		queueJob(uri, false);
	}

	public void insertPresetCopyBelow(IPreset p, IPreset actionAnchor) {
		StaticURI.Path path = new StaticURI.Path("presets", "banks", "copy-preset-below");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("presetToCopy", p.getUUID()), new StaticURI.KeyValue("anchor",
				actionAnchor.getUUID()));
		queueJob(uri, false);
	}

	public void insertEditBufferAbove(IPreset actionAnchor) {
		String uuid = Uuid.random();
		StaticURI.Path path = new StaticURI.Path("presets", "banks", "insert-editbuffer-above");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("anchor", actionAnchor.getUUID()), new StaticURI.KeyValue("uuid", uuid));
		queueJob(uri, false);
		RenameDialog.awaitNewPreset(uuid);
	}

	public void insertEditBufferBelow(IPreset actionAnchor) {
		String uuid = Uuid.random();
		StaticURI.Path path = new StaticURI.Path("presets", "banks", "insert-editbuffer-below");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("anchor", actionAnchor.getUUID()), new StaticURI.KeyValue("uuid", uuid));
		queueJob(uri, false);
		RenameDialog.awaitNewPreset(uuid);
	}

	public void overwritePresetWithEditBuffer(IPreset actionAnchor) {
		StaticURI.Path path = new StaticURI.Path("presets", "banks", "overwrite-preset-with-editbuffer");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("presetToOverwrite", actionAnchor.getUUID()));
		queueJob(uri, false);

		if (actionAnchor.getUUID() != nonMaps.getNonLinearWorld().getParameterEditor().getLoadedPresetUUID())
			RenameDialog.awaitNewPreset(actionAnchor.getUUID());
	}

	public void deletePreset(IPreset preset) {
		StaticURI.Path path = new StaticURI.Path("presets", "banks", "delete-preset");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("uuid", preset.getUUID()));
		queueJob(uri, false);
	}

	public void deletePresets(String csv) {
		StaticURI.Path path = new StaticURI.Path("presets", "banks", "delete-presets");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("presets", csv));
		queueJob(uri, false);
	}

	public void deleteBank(Bank b) {
		StaticURI.Path path = new StaticURI.Path("presets", "delete-bank");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("uuid", b.getUUID()));
		queueJob(uri, false);
	}

	public void loadParameterDescription(final Parameter selectedParameter, final ParameterInfoDialog client) {
		downloadFile("/resources/parameter-descriptions/" + selectedParameter.getParameterID() + ".txt", new DownloadHandler() {

			@Override
			public void onFileDownloaded(String text) {
				client.setDescription(text);
			}

			@Override
			public void onError() {
				client.setDescription("");
			}
		});
	}

	public void appendPreset(IPreset srcPreset, Bank targetBank) {
		StaticURI.Path path = new StaticURI.Path("presets", "banks", "append-preset-to-bank");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("bank-uuid", targetBank.getUUID()), new StaticURI.KeyValue(
				"preset-uuid", srcPreset.getUUID()));
		queueJob(uri, false);
	}

	public void appendEditBuffer(IBank bank) {
		String uuid = Uuid.random();
		StaticURI.Path path = new StaticURI.Path("presets", "banks", "append-preset");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("bank-uuid", bank.getUUID()), new StaticURI.KeyValue("uuid", uuid));
		queueJob(uri, false);
		RenameDialog.awaitNewPreset(uuid);
	}

	public void nextPreset() {
		StaticURI.Path path = new StaticURI.Path("presets", "banks", "next-preset");
		StaticURI uri = new StaticURI(path);
		queueJob(uri, false);
	}

	public void prevPreset() {
		StaticURI.Path path = new StaticURI.Path("presets", "banks", "prev-preset");
		StaticURI uri = new StaticURI(path);
		queueJob(uri, false);
	}

	public void undo() {
		queueJob(new StaticURI(new StaticURI.Path("undo", "undo")), false);
	}

	public void redo() {
		queueJob(new StaticURI(new StaticURI.Path("undo", "redo")), false);
	}

	public void insertPreset(IPreset selPreset) {
		if (selPreset == null) {
			appendPreset();
		} else {
			String uuidOfSelectedPreset = selPreset.getUUID();
			String uuid = Uuid.random();
			StaticURI.Path path = new StaticURI.Path("presets", "banks", "insert-preset");
			StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("uuid", uuid), new StaticURI.KeyValue("seluuid",
					uuidOfSelectedPreset));
			queueJob(uri, false);
			RenameDialog.awaitNewPreset(uuid);
		}
	}

	public void appendPreset() {
		String uuid = Uuid.random();
		StaticURI.Path path = new StaticURI.Path("presets", "banks", "append-preset");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("uuid", uuid));
		queueJob(uri, false);
		RenameDialog.awaitNewPreset(uuid);
	}

	public void setModulationAmount(final ModulatableParameter modulatableParameter) {
		StaticURI.Path path = new StaticURI.Path("presets", "param-editor", "set-mod-amount");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("amount", modulatableParameter.getModulationAmount()
				.getQuantizedClipped()));
		queueJob(uri, true);
	}

	public void setModulationSource(ModulatableParameter modulatableParameter) {
		StaticURI.Path path = new StaticURI.Path("presets", "param-editor", "set-mod-src");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("source", modulatableParameter.getModulationSource().toInt()));
		queueJob(uri, true);
	}

	public void log(String message) {
		StaticURI.Path path = new StaticURI.Path("logger", "log");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("message", message));
		queueJob(uri, true);
	}

	public void setSetting(final String key, final String value) {
		setSetting(key, value, true);
	}

	public void setSetting(final String key, final String value, boolean isOracle) {
		StaticURI.Path path = new StaticURI.Path("settings", "set-setting");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("key", key), new StaticURI.KeyValue("value", value));
		queueJob(uri, isOracle);
	}

	public void resetEditBuffer() {
		StaticURI.Path path = new StaticURI.Path("presets", "param-editor", "reset");
		StaticURI uri = new StaticURI(path);
		queueJob(uri, false);
	}

	public static Node getChild(Node node, String... path) {
		return getChild(node, 0, path);
	}

	private static Node getChild(Node node, int offset, String... path) {

		if (offset == path.length)
			return node;

		NodeList nl = node.getChildNodes();
		int count = nl.getLength();

		for (int i = 0; i < count; i++) {
			Node n = nl.item(i);
			String name = n.getNodeName();
			if (name.equals(path[offset])) {
				return getChild(n, offset + 1, path);
			}
		}

		return null;
	}

	public static String getChildText(Node node, String... path) {
		Node n = getChild(node, path);
		return getText(n);
	}

	public static String getText(Node n) {
		try {
			return n.getChildNodes().item(0).getNodeValue();
		} catch (Exception e) {
			return "";
		}
	}

	public static boolean didChange(Node n) {
		return n.getAttributes().getNamedItem("changed").getNodeValue().equals("1");
	}

	public interface DownloadHandler {
		void onFileDownloaded(String text);

		void onError();
	}

	public void downloadFile(final String url, final DownloadHandler handler) {
		final XMLHttpRequest xhr = XMLHttpRequest.create();
		xhr.open("GET", url);
		xhr.setOnReadyStateChange(new ReadyStateChangeHandler() {

			@Override
			public void onReadyStateChange(XMLHttpRequest xhr) {
				if (xhr.getReadyState() == 4) {
					xhr.clearOnReadyStateChange();

					if (xhr.getStatus() == 200) {
						handler.onFileDownloaded(xhr.getResponseText());
					} else {
						handler.onError();
					}
				}
			}
		});

		xhr.send();
	}

	public void getUniquePresetName(String currentName, DownloadHandler handler) {
		downloadFile("/presets/find-unique-preset-name?based-on=" + URL.encodeQueryString(currentName), handler);
	}

	public void getDiff(Preset presetA, Preset presetB, DownloadHandler handler) {
		String aUUID = presetA != null ? presetA.getUUID() : "";
		String bUUID = presetB != null ? presetB.getUUID() : "";

		downloadFile("/presets/get-diff?p1=" + URL.encodeQueryString(aUUID) + "&p2=" + URL.encodeQueryString(bUUID), handler);
	}

	public void dropPresetOnBank(IPreset p, Bank b) {
		if (b.getPresetList().findPreset(p.getUUID()) != null)
			movePresetBelow(p, b.getLast());
		else
			appendPreset(p, b);
	}

	public void dropPresetsOnBank(String csv, Bank targetBank) {
		StaticURI.Path path = new StaticURI.Path("presets", "banks", "drop-presets-on-bank");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("bank", targetBank.getUUID()), new StaticURI.KeyValue("presets", csv));
		queueJob(uri, false);
	}

	public void dropEditBufferOnBank(IBank b) {
		appendEditBuffer(b);
	}

	public void dropBankOnBank(IBank dragged, IBank b) {
		StaticURI.Path path = new StaticURI.Path("presets", "banks", "drop-bank-on-bank");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("receiver", b.getUUID()), new StaticURI.KeyValue("bank",
				dragged.getUUID()));
		queueJob(uri, false);
	}

	public void insertBankAbove(IBank bank, IPreset preset) {
		StaticURI.Path path = new StaticURI.Path("presets", "banks", "insert-bank-above");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("bank", bank.getUUID()), new StaticURI.KeyValue("anchor",
				preset.getUUID()));
		queueJob(uri, false);
	}

	public void insertBankBelow(IBank bank, IPreset preset) {
		StaticURI.Path path = new StaticURI.Path("presets", "banks", "insert-bank-below");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("bank", bank.getUUID()), new StaticURI.KeyValue("anchor",
				preset.getUUID()));
		queueJob(uri, false);
	}

	public void overwritePresetWithBank(IBank bank, IPreset preset) {
		StaticURI.Path path = new StaticURI.Path("presets", "banks", "overwrite-preset-with-bank");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("bank", bank.getUUID()), new StaticURI.KeyValue("anchor",
				preset.getUUID()));
		queueJob(uri, false);
	}

	public void renameMacroControl(MacroControlParameter param, String newName) {
		StaticURI.Path path = new StaticURI.Path("presets", "param-editor", "rename-mc");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("id", param.getParameterID()), new StaticURI.KeyValue("new-name",
				newName));
		queueJob(uri, false);
	}

	public void uploadReaktorPreset(String preset) {
		StaticURI.Path path = new StaticURI.Path("presets", "param-editor", "import-reaktor-preset");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("preset", preset));
		queueJob(uri, false);
	}

	public void setRibbonReturnMode(int ribbonsParameterId, String mode) {
		StaticURI.Path path = new StaticURI.Path("presets", "param-editor", "set-ribbon-return-mode");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("id", ribbonsParameterId), new StaticURI.KeyValue("mode", mode));
		queueJob(uri, false);
	}

	public void setPedalMode(int pedalParameterId, String mode) {
		StaticURI.Path path = new StaticURI.Path("presets", "param-editor", "set-pedal-mode");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("id", pedalParameterId), new StaticURI.KeyValue("mode", mode));
		queueJob(uri, false);
	}

	public void resetModulation(MacroControlParameter param) {
		StaticURI.Path path = new StaticURI.Path("presets", "param-editor", "reset-modulation");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("id", param.getParameterID()));
		queueJob(uri, false);
	}

	public void importBank(final String fileName, String text, final NonPosition p) {
		final Document xml = XMLParser.parse(text);
		final Node root = xml.getFirstChild();
		final NamedNodeMap attributes = root.getAttributes();

		if (attributes == null || attributes.getNamedItem("version") == null) {
			final ChooseFileVersionDialog d = new ChooseFileVersionDialog();
			d.show(new Runnable() {

				@Override
				public void run() {
					Element e = (Element) root;
					e.setAttribute("version", d.getChoice());
					doImportBank(fileName, xml.toString(), p);
				}
			});

		} else {

			try {
				String versionString = attributes.getNamedItem("version").getNodeValue();
				int fileVersion = Integer.parseInt(versionString);
				if (fileVersion > NonMaps.theMaps.getNonLinearWorld().getPresetManager().getCurrentFileVersion()) {
					Window.alert("Unsupported file version. Please update your C15 firmware.");
					return;
				}
			} catch (Exception e) {
			}

			doImportBank(fileName, text, p);
		}
	}

	private void doImportBank(String fileName, String text, NonPosition p) {
		StaticURI.Path path = new StaticURI.Path("presets", "banks", "import-bank");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("xml", text), new StaticURI.KeyValue("x", p.getX()),
				new StaticURI.KeyValue("y", p.getY()), new StaticURI.KeyValue("fileName", fileName));
		final XMLHttpRequest xhr = XMLHttpRequest.create();
		xhr.open("POST", path.toString());
		xhr.send(uri.getPostData(false));
	}

	public void setBankAttribute(Bank theBank, String key, String value) {
		StaticURI.Path path = new StaticURI.Path("presets", "banks", "set-bank-attribute");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("uuid", theBank.getUUID()), new StaticURI.KeyValue("key", key),
				new StaticURI.KeyValue("value", value));
		queueJob(uri, false);
	}

	public void undoJump(long id) {
		StaticURI.Path path = new StaticURI.Path("undo", "undo-jump");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("target", id));
		queueJob(uri, false);
	}

	public void eraseUndoBranch(long id) {
		StaticURI.Path path = new StaticURI.Path("undo", "erase-branch");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("id", id));
		queueJob(uri, false);
	}

	public void rebaseUndo(long id) {
		StaticURI.Path path = new StaticURI.Path("undo", "rebase");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("id", id));
		queueJob(uri, false);
	}

	public void duplicateBank(Bank bank) {
		StaticURI.Path path = new StaticURI.Path("presets", "banks", "duplicate-bank");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("uuid", bank.getUUID()));
		queueJob(uri, false);
	}

	public void sortBank(Bank bank, boolean asc) {
		StaticURI.Path path = new StaticURI.Path("presets", "banks", "sort-bank");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("uuid", bank.getUUID()), new StaticURI.KeyValue("asc", asc));
		queueJob(uri, false);
	}

	public void searchPresets(String query, SearchQueryCombination combination, String fields, DownloadHandler handler) {
		downloadFile("/presets/search-preset?combine=" + combination.name() + "&query=" + URL.encodeQueryString(query) + "&fields="
				+ fields, handler);
	}

	public void syncLPCToBBB() {
		StaticURI.Path path = new StaticURI.Path("presets", "param-editor", "sync-lpc");
		StaticURI uri = new StaticURI(path);
		queueJob(uri, false);
	}

	public void initSound() {
		StaticURI.Path path = new StaticURI.Path("presets", "param-editor", "init-sound");
		StaticURI uri = new StaticURI(path);
		queueJob(uri, false);
	}

	public void randomizeSound() {
		StaticURI.Path path = new StaticURI.Path("presets", "param-editor", "randomize-sound");
		StaticURI uri = new StaticURI(path);
		queueJob(uri, false);
	}

	public void storeAsInit() {
		StaticURI.Path path = new StaticURI.Path("presets", "store-init");
		StaticURI uri = new StaticURI(path);
		queueJob(uri, false);
	}

	public void resetInit() {
		StaticURI.Path path = new StaticURI.Path("presets", "reset-init");
		StaticURI uri = new StaticURI(path);
		queueJob(uri, false);
	}

	public void setRandomAmount(double r) {
		setSetting("RandomizeAmount", Double.toString(r));
	}

	public void setTransitionTime(double t) {
		setSetting("TransitionTime", Double.toString(t));
	}

	public void setMacroControlInfo(int parameterID, String text) {
		StaticURI.Path path = new StaticURI.Path("presets", "param-editor", "set-macrocontrol-info");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("id", parameterID), new StaticURI.KeyValue("info", text));
		queueJob(uri, false);
	}

	public void setPresetAttribute(Preset thePreset, String key, String value) {
		StaticURI.Path path = new StaticURI.Path("presets", "banks", "set-preset-attribute");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("uuid", thePreset.getUUID()), new StaticURI.KeyValue("key", key),
				new StaticURI.KeyValue("value", value));
		queueJob(uri, false);
	}
	
	public void setEditBufferAttribute(String key, String value) {
		StaticURI.Path path = new StaticURI.Path("presets", "banks", "set-editbuffer-attribute");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("key", key),
				new StaticURI.KeyValue("value", value));
		queueJob(uri, false);
	}

	public void setModulationAmountAndValue(ModulatableParameter param, double newModAmount, double newValue) {
		StaticURI.Path path = new StaticURI.Path("presets", "param-editor", "set-modamount-and-value");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("id", param.getParameterID()), new StaticURI.KeyValue("mod-amount",
				newModAmount), new StaticURI.KeyValue("value", newValue));
		queueJob(uri, false);
	}

	public void moveBy(Bank bank, String dir) {
		StaticURI.Path path = new StaticURI.Path("presets", "banks", "move");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("bank", bank.getUUID()), new StaticURI.KeyValue("direction", dir));
		queueJob(uri, false);
	}

	public void copyPreset(Preset preset) {
		StaticURI.Path path = new StaticURI.Path("clipboard", "copy-preset");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("preset", preset.getUUID()));
		queueJob(uri, false);
	}

	public void copyBank(Bank bank) {
		StaticURI.Path path = new StaticURI.Path("clipboard", "copy-bank");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("bank", bank.getUUID()));
		queueJob(uri, false);
	}

	public void pasteOnPreset(Preset preset) {
		StaticURI.Path path = new StaticURI.Path("clipboard", "paste-on-preset");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("preset", preset.getUUID()));
		queueJob(uri, false);
	}

	public void pasteOnBank(Bank bank) {
		StaticURI.Path path = new StaticURI.Path("clipboard", "paste-on-bank");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("bank", bank.getUUID()));
		queueJob(uri, false);
	}

	public void pasteOnPresetManager(NonPosition pos) {
		StaticURI.Path path = new StaticURI.Path("clipboard", "paste-on-background");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("x", pos.getX()), new StaticURI.KeyValue("y", pos.getY()));
		queueJob(uri, false);
	}

	public void copyPresets(String csv) {
		StaticURI.Path path = new StaticURI.Path("clipboard", "copy-presets");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("presets-csv", csv));
		queueJob(uri, false);
	}
		
	public void cutPreset(Preset preset) {
		StaticURI.Path path = new StaticURI.Path("clipboard", "cut-preset");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("preset", preset.getUUID()));
		queueJob(uri, false);
	}

	public native void importPresetManager(JavaScriptObject buffer) /*-{

		var oReq = new XMLHttpRequest();
		oReq.open("POST", "/presets/import-all-banks", true);
		oReq.setRequestHeader("Content-Type", "application/binary");

		oReq.onreadystatechange = function() {
			if (oReq.readyState == 4 && oReq.status == 200) {
				var ret = oReq.responseText;
				var sub = "Invalid";
				if (ret.includes(sub)) {
					alert(oReq.responseText);
				}
			}
		}

		var blob = new Blob([ buffer ]);
		oReq.send(blob);
	}-*/;

	public void onBankClusterMoved(List<Bank> changedBanks) {
		String csv = "";
		for (Bank curr : changedBanks) {
			if (!csv.isEmpty())
				csv += ",";

			csv += curr.getUUID() + "," + curr.getNonPosition().getPosition().getX() + "," + curr.getNonPosition().getPosition().getY();
		}

		StaticURI.Path path = new StaticURI.Path("presets", "move-cluster");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("csv", csv));
		queueJob(uri, false);
	}

	public void undockBank(Bank slave) {
		StaticURI.Path path = new StaticURI.Path("presets", "banks", "undock-bank");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("uuid", slave.getUUID()), new StaticURI.KeyValue("x", slave
				.getNonPosition().getPosition().getX()), new StaticURI.KeyValue("y", slave.getNonPosition().getPosition().getY()));
		queueJob(uri, false);
	}

	public void dockBank(Bank slave, Bank master, String direction) {
		StaticURI.Path path = new StaticURI.Path("presets", "banks", "dock-bank");

		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("uuid", slave.getUUID()), new StaticURI.KeyValue("master-uuid",
				master.getUUID()), new StaticURI.KeyValue("direction", direction));

		queueJob(uri, false);
	}

	public void incDisplayTest() {
		StaticURI.Path path = new StaticURI.Path("settings", "inc-test-display");
		StaticURI uri = new StaticURI(path);
		queueJob(uri, false);
	}

	public void unlockAllGroups() {
		StaticURI.Path path = new StaticURI.Path("presets", "param-editor", "unlock-all-groups");
		StaticURI uri = new StaticURI(path);
		queueJob(uri, false);
	}

	public void lockAllGroups() {
		StaticURI.Path path = new StaticURI.Path("presets", "param-editor", "lock-all-groups");
		StaticURI uri = new StaticURI(path);
		queueJob(uri, false);
	}

	public void toggleGroupLock(String id) {
		StaticURI.Path path = new StaticURI.Path("presets", "param-editor", "toggle-group-lock");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("group", id));
		queueJob(uri, false);
	}

	public void moveAllBanks(NonDimension distance) {
		StaticURI.Path path = new StaticURI.Path("presets", "banks", "move-all-banks");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("x", distance.getWidth()), new StaticURI.KeyValue("y",
				distance.getHeight()));
		queueJob(uri, false);
	}

	public void dockBanks(Bank droppedOnto, Orientation droppedAt, Bank dragged, NonPosition pos) {
		StaticURI.Path path = new StaticURI.Path("presets", "banks", "dock-banks");

		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("droppedOntoBank", droppedOnto.getUUID()), new StaticURI.KeyValue(
				"draggedBank", dragged.getUUID()), new StaticURI.KeyValue("droppedAt", droppedAt.name()), new StaticURI.KeyValue("x",
				pos.getX()), new StaticURI.KeyValue("y", pos.getY()));

		queueJob(uri, false);

	}

	public void resetScaling() {
		StaticURI.Path path = new StaticURI.Path("presets", "param-editor", "reset-scale");
		StaticURI uri = new StaticURI(path);
		queueJob(uri, false);
	}
	
	public void downloadPreset(String uuid, DownloadHandler downloadHandler) {
		downloadFile("/presets/banks/download-preset/?uuid=" + URL.encodeQueryString(uuid), downloadHandler);
	}

	public void loadEditBuffer(Document xml) {
		StaticURI.Path path = new StaticURI.Path("presets", "load-editbuffer");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("xml", xml.toString()));
		final XMLHttpRequest xhr = XMLHttpRequest.create();
		xhr.open("POST", path.toString());
		xhr.send(uri.getPostData(false));
	}

	public void renameEditBuffer(String text) {
		StaticURI.Path path = new StaticURI.Path("presets", "banks", "rename-editbuffer");
		StaticURI uri = new StaticURI(path, new StaticURI.KeyValue("name", text));
		queueJob(uri, false);
	}

	public void sortBankNumbers() {
		StaticURI.Path path = new StaticURI.Path("presets", "banks", "sort-bank-numbers");
		StaticURI uri = new StaticURI(path);
		queueJob(uri, false);
	}
}
