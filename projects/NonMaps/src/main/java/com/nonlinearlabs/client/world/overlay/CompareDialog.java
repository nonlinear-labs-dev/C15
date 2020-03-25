package com.nonlinearlabs.client.world.overlay;

import java.util.ArrayList;
import java.util.LinkedList;

import com.google.gwt.event.dom.client.ClickEvent;
import com.google.gwt.event.dom.client.ClickHandler;
import com.google.gwt.safehtml.shared.SafeHtmlUtils;
import com.google.gwt.user.client.ui.Button;
import com.google.gwt.user.client.ui.FlexTable;
import com.google.gwt.user.client.ui.HTMLPanel;
import com.google.gwt.user.client.ui.ListBox;
import com.google.gwt.user.client.ui.RootPanel;
import com.google.gwt.xml.client.Document;
import com.google.gwt.xml.client.Element;
import com.google.gwt.xml.client.Node;
import com.google.gwt.xml.client.NodeList;
import com.google.gwt.xml.client.XMLParser;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.ServerProxy;
import com.nonlinearlabs.client.ServerProxy.DownloadHandler;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.SoundType;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.VoiceGroup;
import com.nonlinearlabs.client.presenters.EditBufferPresenterProvider;
import com.nonlinearlabs.client.world.maps.parameters.PhysicalControlParameter.ReturnMode;
import com.nonlinearlabs.client.world.maps.parameters.PlayControls.MacroControls.Macros.MacroControls;
import com.nonlinearlabs.client.world.maps.presets.bank.preset.Preset;

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
	private ListBox selectVGA;
	private ListBox selectVGB;

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

		if (p2 == null)
			addHeader("Compare Preset to Edit Buffer");
		else
			addHeader("Compare Two Presets");

		load(p1, VoiceGroup.I, p2, VoiceGroup.I);
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

	private void load(Preset p1, VoiceGroup vgOfPreset1, Preset p2, VoiceGroup vgOfPreset2) {
		NonMaps.theMaps.getServerProxy().getDiff(p1, vgOfPreset1, p2, vgOfPreset2, new DownloadHandler() {
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
				String colora = colorNode.getAttributes().getNamedItem("a").getNodeValue();
				String colorb = colorNode.getAttributes().getNamedItem("b").getNodeValue();
				if (!colora.isEmpty() && !colorb.isEmpty()) {
					table.setText(row, 0, "Color");
					table.setText(row, 1, colora);
					table.setText(row, 2, colorb);
					row++;
				}
			}

			writeParameterGroups(row);
			setWidget(table);
		}
	}

	ArrayList<String> getParameterGroupOrder() {
		ArrayList<String> order = new ArrayList<>();
		order.add("Envelope A");
		order.add("Envelope B");
		order.add("Envelope C");
		order.add("Oscillator A");
		order.add("Shaper A");
		order.add("Oscillator B");
		order.add("Shaper B");
		order.add("Feedback Mixer");
		order.add("Comb Filter");
		order.add("State Variable Filter");
		order.add("Output Mixer");
		order.add("Flanger");
		order.add("Cabinet");
		order.add("Gap Filter");
		order.add("Echo");
		order.add("Reverb");
		order.add("Master");
		order.add("Mono");
		order.add("Unison");
		order.add("Macro Control");
		order.add("Scale");
		order.add("Part");
		return order;
	}

	Node findParameterGroupNodeByName(NodeList list, String name) {
		for (int numGroup = 0; numGroup < list.getLength(); numGroup++) {
			Node node = list.item(numGroup);
			if (node.getNodeType() == Node.ELEMENT_NODE)
				if (node.getAttributes().getNamedItem("name").getNodeValue().equals(name))
					return node;
		}
		return null;
	}

	public void writeParameterGroups(int row) {
		NodeList groups = xml.getElementsByTagName("group");

		for (String id : getParameterGroupOrder()) {
			Node group = findParameterGroupNodeByName(groups, id);
			if (group != null)
				row = writeParameterGroup(row, group);
		}
	}

	public int writeParameterGroup(int row, Node group) {
		table.setText(row, 0, group.getAttributes().getNamedItem("name").getNodeValue());
		row++;
		int startedAtRow = row;

		NodeList params = group.getChildNodes();

		int numParams = params.getLength();

		for (int numParam = 0; numParam < numParams; numParam++) {
			Node param = params.item(numParam);

			if (param.getNodeType() == Node.ELEMENT_NODE) {
				row = writeParameter(row, param);
			}
		}

		if (row == startedAtRow) {
			table.removeRow(row - 1);
			row--;
		}
		return row;
	}

	public int writeParameter(int row, Node param) {
		NodeList changes = param.getChildNodes();
		LinkedList<Node> elements = new LinkedList<Node>();
		int numChanges = changes.getLength();

		for (int numChange = 0; numChange < numChanges; numChange++) {
			Node change = changes.item(numChange);

			if (change.getNodeType() == Node.ELEMENT_NODE) {
				elements.add(change);
			}
		}

		if (!elements.isEmpty()) {
			String paramName = param.getAttributes().getNamedItem("name").getNodeValue();
			table.setWidget(row, 0, new HTMLPanel("span", indent + paramName));
			table.getWidget(row, 0).getElement().addClassName("indent-1");
			row++;

			for (Node change : elements)
				row = writeParameterChange(row, paramName, change);
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
				refresh();
			}
		});

		String preset1PositionText = SafeHtmlUtils
				.htmlEscape(positionNode.getAttributes().getNamedItem("a").getNodeValue());
		String preset2PositionText = SafeHtmlUtils
				.htmlEscape(positionNode.getAttributes().getNamedItem("b").getNodeValue());

		String preset1Name = nameNode.getAttributes().getNamedItem("a").getNodeValue();
		String preset2Name = nameNode.getAttributes().getNamedItem("b").getNodeValue();

		table.setHTML(row, 1, preset1PositionText + "<br>" + SafeHtmlUtils.htmlEscape(preset1Name));
		table.setHTML(row, 2, preset2PositionText + "<br>" + SafeHtmlUtils.htmlEscape(preset2Name));
		row++;
		addLoadButtons(row, table);
		row++;
		addVoiceGroupMenus(row, table);
		row++;

		return row;
	}

	protected void refresh() {
		VoiceGroup vgA = selectVGA.getSelectedIndex() == 0 ? VoiceGroup.I : VoiceGroup.II;
		VoiceGroup vgB = selectVGB.getSelectedIndex() == 0 ? VoiceGroup.I : VoiceGroup.II;
		load(presetA, vgA, presetB, vgB);
	}

	public void update() {
	}

	private void updateLoadButtonStates() {
		if (xml != null) {
			Element root = xml.getDocumentElement();
			Node enabled = ServerProxy.getChild(root, "enabled");
			String a = enabled.getAttributes().getNamedItem("a").getNodeValue();
			String b = enabled.getAttributes().getNamedItem("b").getNodeValue();

			loadPresetA.setEnabled(a.equals("1"));
			loadPresetB.setEnabled(b.equals("1"));
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
				NonMaps.get().getServerProxy().loadPresetFromXML(presetAXml);
				loadPresetA.setEnabled(false);
				loadPresetB.setEnabled(true);
			}
		});

		loadPresetB.addClickHandler(new ClickHandler() {

			@Override
			public void onClick(ClickEvent event) {
				NonMaps.get().getServerProxy().loadPresetFromXML(presetBXml);
				loadPresetA.setEnabled(true);
				loadPresetB.setEnabled(false);
			}
		});
	}

	public void addVoiceGroupMenus(int row, FlexTable table) {
		int selA = selectVGA != null ? selectVGA.getSelectedIndex() : 0;
		int selB = selectVGB != null ? selectVGB.getSelectedIndex() : 0;

		table.setWidget(row, 1, selectVGA = new ListBox());
		table.setWidget(row, 2, selectVGB = new ListBox());

		selectVGA.getElement().addClassName("voicegroup-menu");
		selectVGB.getElement().addClassName("voicegroup-menu");

		selectVGA.addItem("Part I");
		selectVGB.addItem("Part I");

		SoundType ebType = EditBufferPresenterProvider.getPresenter().soundType;

		if (presetA != null && presetA.getType() != SoundType.Single)
			selectVGA.addItem("Part II");
		if (presetA == null && ebType != SoundType.Single)
			selectVGA.addItem("Part II");

		if (presetB != null && presetB.getType() != SoundType.Single)
			selectVGB.addItem("Part II");
		if (presetB == null && ebType != SoundType.Single)
			selectVGB.addItem("Part II");

		if (selectVGA.getItemCount() > selA)
			selectVGA.setSelectedIndex(selA);

		if (selectVGB.getItemCount() > selB)
			selectVGB.setSelectedIndex(selB);

		if (selectVGA.getItemCount() < 2)
			selectVGA.setEnabled(false);

		if (selectVGB.getItemCount() < 2)
			selectVGB.setEnabled(false);

		selectVGA.addChangeHandler(v -> {
			refresh();
		});

		selectVGB.addChangeHandler(v -> {
			refresh();
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
