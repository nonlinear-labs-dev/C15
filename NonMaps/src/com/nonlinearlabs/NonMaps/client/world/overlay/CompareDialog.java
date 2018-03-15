package com.nonlinearlabs.NonMaps.client.world.overlay;

import com.google.gwt.event.dom.client.ClickEvent;
import com.google.gwt.event.dom.client.ClickHandler;
import com.google.gwt.safehtml.shared.SafeHtmlUtils;
import com.google.gwt.user.client.ui.Button;
import com.google.gwt.user.client.ui.FlexTable;
import com.google.gwt.user.client.ui.HTMLPanel;
import com.google.gwt.user.client.ui.RootPanel;
import com.google.gwt.xml.client.Document;
import com.google.gwt.xml.client.Element;
import com.google.gwt.xml.client.Node;
import com.google.gwt.xml.client.NodeList;
import com.google.gwt.xml.client.XMLParser;
import com.nonlinearlabs.NonMaps.client.NonMaps;
import com.nonlinearlabs.NonMaps.client.ServerProxy;
import com.nonlinearlabs.NonMaps.client.ServerProxy.DownloadHandler;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.PhysicalControlParameter.ReturnMode;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.PlayControls.MacroControls.Macros.MacroControls;
import com.nonlinearlabs.NonMaps.client.world.maps.presets.bank.preset.Preset;

public class CompareDialog extends GWTDialog {

	private Document xml;
	private Document presetAXml;
	private Document presetBXml;
	private Button loadPresetA;
	private Button loadPresetB;

	private FlexTable table = null;
	private String indent = "&#9492 ";
	private Button refresh;

	private Preset presetA;
	private Preset presetB;

	public static void open(Preset p1) {
		new CompareDialog(p1);
		NonMaps.get().getNonLinearWorld().getPresetManager().closeMultiSelection();
	}

	public static void open(Preset p1, Preset p2) {
		new CompareDialog(p1, p2);
		NonMaps.get().getNonLinearWorld().getPresetManager().closeMultiSelection();
	}

	private CompareDialog(Preset p) {
		init(p, null);
	}

	private CompareDialog(Preset p1, Preset p2) {
		init(p1, p2);
	}

	private void init(Preset p1, Preset p2) {
		NonMaps.get().getNonLinearWorld().getViewport().getOverlay().addCompareDialog(this);

		RootPanel.get().add(this);

		getElement().addClassName("preset-compare-dialog");

		initalShow();

		setAnimationEnabled(true);
		setGlassEnabled(false);
		setModal(false);

		if(p2 == null)
			addHeader("Compare Preset to Edit Buffer");
		else
			addHeader("Compare two Presets");
		
		load(p1, p2);
	}

	public void downloadPresets(Preset p1, Preset p2) {
		presetA = p1;
		presetB = p2;

		NonMaps.theMaps.getServerProxy().downloadPreset(p1 != null ? p1.getUUID() : "", new DownloadHandler() {
			@Override
			public void onFileDownloaded(String text) {
				presetAXml = XMLParser.parse(text);
				updateLoadButtonStates();
			}

			@Override
			public void onError() {
			}
		});

		NonMaps.theMaps.getServerProxy().downloadPreset(p2 != null ? p2.getUUID() : "", new DownloadHandler() {
			@Override
			public void onFileDownloaded(String text) {
				presetBXml = XMLParser.parse(text);
				updateLoadButtonStates();
			}

			@Override
			public void onError() {
			}
		});
	}

	private void load(Preset p1, Preset p2) {
		NonMaps.theMaps.getServerProxy().getDiff(p1, p2, new DownloadHandler() {
			@Override
			public void onFileDownloaded(String text) {
				xml = XMLParser.parse(text);
				setup();
			}

			@Override
			public void onError() {
			}
		});

		downloadPresets(p1, p2);
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
	protected void commit() {
		hide();
		NonMaps.get().getNonLinearWorld().getViewport().getOverlay().removeCompareDialog(this);
		NonMaps.theMaps.captureFocus();
		NonMaps.theMaps.getNonLinearWorld().requestLayout();
	}

	protected void setup() {
		if (xml != null) {
			Element root = xml.getDocumentElement();
			Node positionNode = ServerProxy.getChild(root, "position");
			Node nameNode = ServerProxy.getChild(root, "name");
			Node commentNode = ServerProxy.getChild(root, "comment");
			Node colorNode = ServerProxy.getChild(root, "color");

			int row = 0;
			table = new FlexTable();
			table.getElement().addClassName("compare-tree");

			row = writeHeader(row, positionNode, nameNode);

			if (commentNode != null) {
				table.setText(row, 0, "Comment");
				table.setText(row, 1, commentNode.getAttributes().getNamedItem("a").getNodeValue());
				table.setText(row, 2, commentNode.getAttributes().getNamedItem("b").getNodeValue());
				row++;
			}

			if (colorNode != null) {
				table.setText(row, 0, "Color");
				table.setText(row, 1, colorNode.getAttributes().getNamedItem("a").getNodeValue());
				table.setText(row, 2, colorNode.getAttributes().getNamedItem("b").getNodeValue());
				row++;
			}

			writeParameterGroups(row);
			setWidget(table);
		}
	}

	public void writeParameterGroups(int row) {
		NodeList groups = xml.getElementsByTagName("group");
		int numGroups = groups.getLength();

		for (int numGroup = 0; numGroup < numGroups; numGroup++) {
			Node group = groups.item(numGroup);

			if (group.getNodeType() == Node.ELEMENT_NODE) {
				row = writeParameterGroup(row, group);
			}
		}
	}

	public int writeParameterGroup(int row, Node group) {
		table.setText(row, 0, group.getAttributes().getNamedItem("name").getNodeValue());
		row++;

		NodeList params = group.getChildNodes();

		int numParams = params.getLength();

		for (int numParam = 0; numParam < numParams; numParam++) {
			Node param = params.item(numParam);

			if (param.getNodeType() == Node.ELEMENT_NODE) {
				row = writeParameter(row, param);
			}
		}
		return row;
	}

	public int writeParameter(int row, Node param) {
		String paramName = param.getAttributes().getNamedItem("name").getNodeValue();
		table.setWidget(row, 0, new HTMLPanel("span", indent + paramName));
		table.getWidget(row, 0).getElement().addClassName("indent-1");
		row++;

		NodeList changes = param.getChildNodes();

		int numChanges = changes.getLength();

		for (int numChange = 0; numChange < numChanges; numChange++) {
			Node change = changes.item(numChange);

			if (change.getNodeType() == Node.ELEMENT_NODE) {
				row = writeParameterChange(row, paramName, change);
			}
		}
		return row;
	}

	public int writeParameterChange(int row, String paramName, Node change) {
		if (change.getNodeName().equals("value")) {
			table.setText(row - 1, 1, change.getAttributes().getNamedItem("a").getNodeValue());
			table.setText(row - 1, 2, change.getAttributes().getNamedItem("b").getNodeValue());
		} else {
			table.setWidget(row, 0, new HTMLPanel("span", indent + translateChangeName(change.getNodeName())));
			table.getWidget(row, 0).getElement().addClassName("indent-2");
			String aValue = change.getAttributes().getNamedItem("a").getNodeValue();
			String bValue = change.getAttributes().getNamedItem("b").getNodeValue();
			table.setText(row, 1, translateChangeValue(paramName, change.getNodeName(), aValue));
			table.setText(row, 2, translateChangeValue(paramName, change.getNodeName(), bValue));
			row++;
		}
		return row;
	}

	public int writeHeader(int row, Node positionNode, Node nameNode) {

		table.setWidget(row, 0, refresh = new Button(""));
		refresh.getElement().addClassName("refresh-button");

		refresh.addClickHandler(new ClickHandler() {

			@Override
			public void onClick(ClickEvent event) {
				load(presetA, presetB);
			}
		});

		String preset1PositionText = SafeHtmlUtils.htmlEscape(positionNode.getAttributes().getNamedItem("a").getNodeValue());
		String preset2PositionText = SafeHtmlUtils.htmlEscape(positionNode.getAttributes().getNamedItem("b").getNodeValue());

		String preset1Name = nameNode.getAttributes().getNamedItem("a").getNodeValue();
		String preset2Name = nameNode.getAttributes().getNamedItem("b").getNodeValue();

		table.setHTML(row, 1, preset1PositionText + "<br>" + SafeHtmlUtils.htmlEscape(preset1Name));
		table.setHTML(row, 2, preset2PositionText + "<br>" + SafeHtmlUtils.htmlEscape(preset2Name));
		row++;
		addLoadButtons(row, table);
		row++;
		return row;
	}

	public void update() {
		updateLoadButtonStates();
	}

	private void updateLoadButtonStates() {
		if (xml != null) {
			Element root = xml.getDocumentElement();
			Node hashNode = ServerProxy.getChild(root, "hash");
			String a = hashNode.getAttributes().getNamedItem("a").getNodeValue();
			String b = hashNode.getAttributes().getNamedItem("b").getNodeValue();
			String ebHash = NonMaps.get().getNonLinearWorld().getParameterEditor().getHash();

			loadPresetA.setEnabled(!(presetAXml == null || a.equals(ebHash)));
			loadPresetB.setEnabled(!(presetBXml == null || b.equals(ebHash)));
		}
	}

	public void addLoadButtons(int row, FlexTable table) {
		table.setWidget(row, 1, loadPresetA = new Button(""));
		table.setWidget(row, 2, loadPresetB = new Button(""));

		loadPresetA.getElement().addClassName("load-button");
		loadPresetB.getElement().addClassName("load-button");

		updateLoadButtonStates();

		loadPresetA.addClickHandler(new ClickHandler() {

			@Override
			public void onClick(ClickEvent event) {
				NonMaps.get().getServerProxy().loadEditBuffer(presetAXml);
			}
		});

		loadPresetB.addClickHandler(new ClickHandler() {

			@Override
			public void onClick(ClickEvent event) {
				NonMaps.get().getServerProxy().loadEditBuffer(presetBXml);
			}
		});
	}

	private String translateChangeValue(String paramName, String nodeName, String nodeValue) {
		switch (nodeName) {
		case "mc-select":
			return MacroControls.fromInt(Integer.parseInt(nodeValue)).toPrettyString();

		case "return-mode":
			return ReturnMode.fromInt(Integer.parseInt(nodeValue)).toString();

		case "name":
			return nodeValue;

		case "info":
			return nodeValue;

		case "behaviour":
			int b = Integer.parseInt(nodeValue);
			if (b == 0)
				return "Absolute";
			return "Relative";
		}
		return nodeValue;
	}

	private String translateChangeName(String nodeName) {
		switch (nodeName) {
		case "mc-amount":
			return "MC Amount";

		case "mc-select":
			return "MC Select";

		case "return-mode":
			return "Return Mode";

		case "name":
			return "Name";

		case "info":
			return "Info";

		case "behaviour":
			return "Behaviour";
		}
		return nodeName;
	}
}
