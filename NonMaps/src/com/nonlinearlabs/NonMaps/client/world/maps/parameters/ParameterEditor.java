package com.nonlinearlabs.NonMaps.client.world.maps.parameters;

import java.util.HashMap;
import java.util.HashSet;

import com.google.gwt.event.dom.client.KeyDownEvent;
import com.google.gwt.xml.client.Node;
import com.google.gwt.xml.client.NodeList;
import com.nonlinearlabs.NonMaps.client.NonMaps;
import com.nonlinearlabs.NonMaps.client.ServerProxy;
import com.nonlinearlabs.NonMaps.client.Tracer;
import com.nonlinearlabs.NonMaps.client.dataModel.editBuffer.EditBufferModel;
import com.nonlinearlabs.NonMaps.client.world.AppendOverwriteInsertPresetDialog;
import com.nonlinearlabs.NonMaps.client.world.Control;
import com.nonlinearlabs.NonMaps.client.world.Name;
import com.nonlinearlabs.NonMaps.client.world.NonLinearWorld;
import com.nonlinearlabs.NonMaps.client.world.maps.LayoutResizingHorizontal;
import com.nonlinearlabs.NonMaps.client.world.maps.LayoutResizingVertical;
import com.nonlinearlabs.NonMaps.client.world.maps.MapsLayout;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.Parameter.Initiator;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.Cabinet.Cabinet;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.CombFilter.CombFilter;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.Echo.Echo;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.EnvA.EnvA;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.EnvB.EnvB;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.EnvC.EnvC;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.FBMixer.FBMixer;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.Flanger.Flanger;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.GapFilter.GapFilter;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.Master.Master;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.OscA.OscA;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.OscB.OscB;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.OutputMixer.OutputMixer;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.PlayControls.PlayControls;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.PlayControls.MacroControls.Macros;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.Reverb.Reverb;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.SVFilter.SVFilter;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.Scale.Scale;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.ShapeA.ShapeA;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.ShapeB.ShapeB;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.Unison.Unison;
import com.nonlinearlabs.NonMaps.client.world.maps.presets.PresetManager;
import com.nonlinearlabs.NonMaps.client.world.maps.presets.bank.preset.Preset;
import com.nonlinearlabs.NonMaps.client.world.overlay.CompareDialog;

public class ParameterEditor extends LayoutResizingVertical {

	private String loadedPreset = "";
	private PlayControls playControls;
	private boolean m_isModified = false;
	private static ParameterEditor theEditor = null;
	private HashMap<String, String> attributes = new HashMap<String, String>();
	private String hash = "";

	private class PlayControlsArea extends ResizingHorizontalCenteringLayout {

		private PlayControlsArea(ParameterEditor parent) {
			super(parent);
			playControls = addChild(new PlayControls(this));
		}
	}

	private class SynthParameters extends ResizingHorizontalCenteringLayout {

		private SynthParameters(ParameterEditor parent) {
			super(parent);
			addChild(new ParamEditorCol1());
			addChild(new SpacerLarge(this));
			addChild(new ParamEditorCol2());
			addChild(new SpacerLarge(this));
			addChild(new ParamEditorCol3());
			addChild(new SpacerLarge(this));
			addChild(new ParamEditorCol4());
		}

		private class ParamEditorCol1 extends LayoutResizingVertical {

			public ParamEditorCol1() {
				super(SynthParameters.this);
				addChild(new EnvA(this));
				addChild(new SpacerLarge(this));
				addChild(new EnvC(this));
				addChild(new SpacerLarge(this));
				addChild(new EnvB(this));
			}

			@Override
			public Direction getLayoutDirection() {
				return Direction.RIGHT;
			}

		}

		private class ParamEditorCol2 extends LayoutResizingVertical {

			public ParamEditorCol2() {
				super(SynthParameters.this);
				addChild(new OscA(this));
				addChild(new SpacerLarge(this));
				addChild(new ShapeA(this));
				addChild(new SpacerLarge(this));
				addChild(new ShapeB(this));
				addChild(new SpacerLarge(this));
				addChild(new OscB(this));
			}
		}

		private class ParamEditorCol3 extends LayoutResizingVertical {

			public ParamEditorCol3() {
				super(SynthParameters.this);
				addChild(new FBMixer(this));
				addChild(new SpacerLarge(this));
				addChild(new CombFilter(this));
				addChild(new SpacerLarge(this));
				addChild(new SVFilter(this));
				addChild(new SpacerLarge(this));
				addChild(new OutputMixer(this));
			}
		}

		private class ParamEditorCol4 extends LayoutResizingVertical {

			private class CabinetAndGapFilter extends LayoutResizingHorizontal {

				public CabinetAndGapFilter() {
					super(SynthParameters.this);

					addChild(new Cabinet(this));
					addChild(new SpacerLarge(this));
					addChild(new GapFilter(this));
				}
			}

			private class EchoAndReverb extends LayoutResizingHorizontal {

				public EchoAndReverb() {
					super(SynthParameters.this);

					addChild(new Echo(this));
					addChild(new SpacerLarge(this));
					addChild(new Reverb(this));
				}
			}

			private class UnisonAndMaster extends LayoutResizingHorizontal {

				public UnisonAndMaster() {
					super(SynthParameters.this);

					addChild(new Unison(this));
					addChild(new SpacerLarge(this));
					addChild(new Master(this));
					addChild(new SpacerLarge(this));
					addChild(new Scale(this));
				}
			}

			private ParamEditorCol4() {
				super(SynthParameters.this);

				addChild(new Flanger(this));
				addChild(new SpacerLarge(this));
				addChild(new CabinetAndGapFilter());

				addChild(new SpacerLarge(this));
				addChild(new EchoAndReverb());
				addChild(new SpacerLarge(this) {
					@Override
					protected double getBasicWidth() {
						return super.getBasicWidth() * 8;
					}
				});

				addChild(new UnisonAndMaster());
			}

			@Override
			public Direction getLayoutDirection() {
				return Direction.LEFT;
			}
		}
	}

	public ParameterEditor(NonLinearWorld parent) {
		super(parent);

		assert (theEditor == null);
		theEditor = this;

		addChild(new SpacerLarge(this) {
			@Override
			protected double getBasicHeight() {
				return 10;
			}
		});

		addChild(new PlayControlsArea(this));
		addChild(new SpacerLarge(this));
		addChild(new SpacerLarge(this));
		addChild(new SynthParameters(this));
	}

	public Control onKey(final KeyDownEvent event) {

		int keyCode = event.getNativeEvent().getKeyCode();

		if (keyCode == com.google.gwt.event.dom.client.KeyCodes.KEY_E && getNonMaps().getNonLinearWorld().isCtrlDown()) {
			toggleVisibility();
			return this;
		}
		return null;
	}

	public static ParameterEditor get() {
		return theEditor;
	}

	private Parameter selectedObject = null;

	public void select(Initiator initiator, Parameter sel) {
		if (selectedObject != sel) {
			Parameter oldSel = selectedObject;
			selectedObject = sel;

			for (SelectionListener listener : listeners) {
				listener.onSelectionChanged(oldSel, sel);
			}

			if (initiator == Initiator.EXPLICIT_USER_ACTION)
				getNonMaps().getServerProxy().onParameterSelectionChanged(this);

			if (NonMaps.theMaps.getNonLinearWorld().getSettings().isOneOf("SelectionAutoScroll", "on", "parameter", "parameter-and-preset"))
				scrollToSelectedParameter();

			invalidate(INVALIDATION_FLAG_UI_CHANGED);
		}
	}

	private void scrollToSelectedParameter() {
		if (!selectedObject.isVisible())
			((MapsLayout) selectedObject.getParameterGroup()).scrollToMakeFullyVisible();
	}

	public Parameter getSelection() {
		return selectedObject;
	}

	private HashSet<SelectionListener> listeners = new HashSet<SelectionListener>();
	private String loadedPresetsName = "Init";

	public void registerListener(SelectionListener listener) {
		listeners.add(listener);
		listener.onSelectionChanged(null, selectedObject);
	}

	public void removeListener(SelectionListener listener) {
		listeners.remove(listener);
	}

	public void update(Node node, boolean omitOracles) {
		if (node == null)
			return;

		if (ServerProxy.didChange(node)) {
			updateHash(node);
			updateCompare(node);

			if (!omitOracles) {
				updateParameterGroups(node);
				updatePreset(node);
				updateSelection(node);
			} else {
				Tracer.log("did not change parameters in parameter editor from update doc, because it says the changes can be omitted");
			}
		}

		String loadedPreset = node.getAttributes().getNamedItem("loaded-preset").getNodeValue();

		if (loadedPreset != null && !loadedPreset.equals(this.loadedPreset)) {
			this.loadedPreset = loadedPreset;
			AppendOverwriteInsertPresetDialog.close();
			invalidate(INVALIDATION_FLAG_UI_CHANGED);
		}

		String loadedPresetsName = node.getAttributes().getNamedItem("loaded-presets-name").getNodeValue();

		if (loadedPresetsName != null && !loadedPresetsName.equals(this.loadedPresetsName)) {
			this.loadedPresetsName = loadedPresetsName;
			invalidate(INVALIDATION_FLAG_UI_CHANGED);
		}

		m_isModified = node.getAttributes().getNamedItem("is-modified").getNodeValue().equals("1");
	}

	public void updateHash(Node node) {
		String hash = node.getAttributes().getNamedItem("hash").getNodeValue();
		if (!hash.equals(this.hash)) {
			this.hash = hash;
		}
	}

	private void updatePreset(Node node) {
		NodeList children = node.getChildNodes();

		for (int i = 0; i < children.getLength(); i++) {
			Node n = children.item(i);
			String nodesName = n.getNodeName();

			if (nodesName.equals("preset")) {
				updateAttributes(n);
			}
		}
	}

	private void updateAttributes(Node node) {
		NodeList children = node.getChildNodes();

		for (int i = 0; i < children.getLength(); i++) {
			Node n = children.item(i);
			String nodesName = n.getNodeName();

			if (nodesName.equals("attribute")) {
				updateAttribute(n);
			}
		}
	}

	private void updateAttribute(Node n) {
		String key = n.getAttributes().getNamedItem("key").getNodeValue();
		String value = ServerProxy.getText(n);
		attributes.put(key, value);
	}

	public String getAttribute(String key) {
		String ret = attributes.get(key);
		if (ret != null)
			return ret;
		return "";
	}

	private void updateParameterGroups(Node node) {
		NodeList parameterGroups = node.getChildNodes();

		for (int i = 0; i < parameterGroups.getLength(); i++) {
			Node n = parameterGroups.item(i);
			String nodesName = n.getNodeName();

			if (nodesName.equals("parameter-group")) {
				updateParameters(n);
			}
		}
	}

	private void updateParameters(Node parameterGroupNode) {
		if (ServerProxy.didChange(parameterGroupNode)) {
			String longGroupName = parameterGroupNode.getAttributes().getNamedItem("long-name").getNodeValue();
			String shortGroupName = parameterGroupNode.getAttributes().getNamedItem("short-name").getNodeValue();
			String groupID = parameterGroupNode.getAttributes().getNamedItem("id").getNodeValue();

			ParameterGroupIface grp = findParameterGroup(groupID);

			if (grp != null)
				grp.setName(new Name(longGroupName, shortGroupName));

			NodeList parameters = parameterGroupNode.getChildNodes();

			for (int i = 0; i < parameters.getLength(); i++) {
				Node n = parameters.item(i);

				if (n.getNodeName().equals("parameter")) {
					updateParameter(n);
				}
			}
		}
	}

	private void updateParameter(Node n) {
		if (ServerProxy.didChange(n)) {
			String id = n.getAttributes().getNamedItem("id").getNodeValue();
			Parameter sel = findSelectable(Integer.parseInt(id));

			if (sel != null) {
				Parameter param = (Parameter) sel;
				param.update(n);
			}
		}
	}

	public ParameterGroupIface findParameterGroup(final String groupID) {
		Control found = recurseChildren(new ControlFinder() {

			@Override
			public boolean onWayDownFound(Control child) {
				if (child instanceof ParameterGroupIface) {
					ParameterGroupIface i = (ParameterGroupIface) child;
					if (i.getID().equals(groupID))
						return true;
				}
				return false;
			}
		});

		return (ParameterGroupIface) found;
	}

	private void updateSelection(Node node) {
		String selected = node.getAttributes().getNamedItem("selected-parameter").getNodeValue();
		Parameter sel = findSelectable(Integer.parseInt(selected));

		if (sel != selectedObject) {
			select(Initiator.INDIRECT_USER_ACTION, sel);
		}
	}

	private void updateCompare(Node node) {
		for (CompareDialog compareDialog : NonMaps.get().getNonLinearWorld().getViewport().getOverlay().getCompareDialogs()) {
			compareDialog.update();
		}
	}

	private HashMap<Integer, Parameter> parameterMap = new HashMap<Integer, Parameter>();

	public void registerSelectable(Parameter sel) {
		Parameter pl = (Parameter) sel;
		parameterMap.put(pl.getParameterID(), pl);
	}

	Parameter findSelectable(int parameterID) {
		return parameterMap.get(parameterID);
	}

	public String getLoadedPresetUUID() {
		return loadedPreset;
	}

	public String getLoadedPresetName() {
		return loadedPresetsName;
	}

	public Macros getMacroControls() {
		return playControls.getMacroControls();
	}

	public Parameter getSelectedOrSome() {
		if (selectedObject != null)
			return (Parameter) selectedObject;

		return parameterMap.values().iterator().next();
	}

	public Parameter findParameter(int parameterID) {
		return (Parameter) findSelectable(parameterID);
	}

	public PlayControls getPlayControls() {
		return playControls;
	}

	public boolean isEditorVisible() {
		return isVisible();
	}

	public void toggleVisibility() {
		setVisible(!isIntendedVisible());
	}

	public String getLoadedPresetInfo() {
		Preset p = NonMaps.get().getNonLinearWorld().getPresetManager().findPreset(loadedPreset);

		if (p != null)
			return p.getAttribute("Comment");

		return "";
	}

	public String getHash() {
		return hash;
	}

	public boolean isAnyParameterLocked() {
		Control c = recurseChildren(new ControlFinder() {
			@Override
			public boolean onWayDownFound(Control ctrl) {
				if (ctrl instanceof Parameter) {
					Parameter p = (Parameter) ctrl;
					if (p.isLocked()) {
						return true;
					}
				}
				return false;
			}
		});

		return c != null;
	}

	public boolean areAllParametersLocked() {
		for (Parameter p : parameterMap.values()) {
			if (p.isLocked() == false)
				return false;
		}
		return true;
	}
}
