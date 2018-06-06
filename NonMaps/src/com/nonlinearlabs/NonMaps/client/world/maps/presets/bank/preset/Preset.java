package com.nonlinearlabs.NonMaps.client.world.maps.presets.bank.preset;

import java.util.HashMap;

import com.google.gwt.canvas.dom.client.Context2d;
import com.google.gwt.i18n.client.NumberFormat;
import com.google.gwt.xml.client.Node;
import com.google.gwt.xml.client.NodeList;
import com.nonlinearlabs.NonMaps.client.NonMaps;
import com.nonlinearlabs.NonMaps.client.Renameable;
import com.nonlinearlabs.NonMaps.client.ServerProxy;
import com.nonlinearlabs.NonMaps.client.StoreSelectMode;
import com.nonlinearlabs.NonMaps.client.Tracer;
import com.nonlinearlabs.NonMaps.client.world.Control;
import com.nonlinearlabs.NonMaps.client.world.IPreset;
import com.nonlinearlabs.NonMaps.client.world.NonLinearWorld;
import com.nonlinearlabs.NonMaps.client.world.Position;
import com.nonlinearlabs.NonMaps.client.world.RGB;
import com.nonlinearlabs.NonMaps.client.world.RGBA;
import com.nonlinearlabs.NonMaps.client.world.Rect;
import com.nonlinearlabs.NonMaps.client.world.RenameDialog;
import com.nonlinearlabs.NonMaps.client.world.maps.LayoutResizingHorizontal;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.Parameter.Initiator;
import com.nonlinearlabs.NonMaps.client.world.maps.presets.MultiplePresetSelection;
import com.nonlinearlabs.NonMaps.client.world.maps.presets.PresetManager;
import com.nonlinearlabs.NonMaps.client.world.maps.presets.bank.Bank;
import com.nonlinearlabs.NonMaps.client.world.overlay.ContextMenu;
import com.nonlinearlabs.NonMaps.client.world.overlay.DragProxy;
import com.nonlinearlabs.NonMaps.client.world.overlay.Overlay;
import com.nonlinearlabs.NonMaps.client.world.overlay.PresetInfoDialog;
import com.nonlinearlabs.NonMaps.client.world.overlay.belt.presets.PresetContextMenu;
import com.nonlinearlabs.NonMaps.client.world.overlay.setup.ContextMenusSetting;

public class Preset extends LayoutResizingHorizontal implements Renameable, IPreset {
	private String uuid = null;
	private ColorTag tag = null;
	private Name name = null;
	private Number number = null;
	private HashMap<String, String> attributes = new HashMap<String, String>();

	public enum FilterState {
		NO_FILTER, FILTER_MATCHES, FILTERED_OUT
	};

	private FilterState filterSate = FilterState.NO_FILTER;

	public Preset(Bank parent) {
		super(parent);

		tag = addChild(new ColorTag(this));
		number = addChild(new Number(this, ""));
		name = addChild(new Name(this, ""));
	}

	@Override
	public Bank getParent() {
		return (Bank) super.getParent();
	}

	@Override
	public RGB getColorFont() {
		boolean selected = isSelected();
		boolean loaded = isLoaded() && !isInStoreSelectMode();
		boolean isOriginPreset = isLoaded() && isInStoreSelectMode();

		if (isInMultiplePresetSelectionMode()) {
			selected = getParent().getParent().getMultiSelection().contains(this);
			loaded = false;
		}

		if (isOriginPreset)
			return new RGB(255, 255, 255);

		if (filterSate == FilterState.FILTER_MATCHES)
			return new RGB(230, 240, 255);
		else if (filterSate == FilterState.FILTERED_OUT)
			return new RGB(179, 179, 179);

		if (!selected && !loaded)
			return new RGB(179, 179, 179);

		return super.getColorFont();
	}

	public void update(int i, Node preset) {
		this.uuid = preset.getAttributes().getNamedItem("uuid").getNodeValue();
		String name = preset.getAttributes().getNamedItem("name").getNodeValue();
		this.number.setText(NumberFormat.getFormat("#000").format(i));
		this.name.setText(name);
		updateAttributes(preset);

		if (isSelected() && getParent().isSelected() && PresetInfoDialog.isShown())
			PresetInfoDialog.update(this);
	}

	public int getNumber() {
		return Integer.parseInt(number.getText());
	}

	public String getPaddedNumber() {
		NumberFormat f = NumberFormat.getFormat("000");
		String ret = f.format(getNumber());
		return ret;
	}

	@Override
	public void doFirstLayoutPass(double levelOfDetail) {

		super.doFirstLayoutPass(levelOfDetail);

		if (getParent().isMinimized()) {
			if (!isSelected()) {
				tag.getNonPosition().getDimension().setHeight(0);
				number.getNonPosition().getDimension().setHeight(0);
				name.getNonPosition().getDimension().setHeight(0);
				getNonPosition().getDimension().setHeight(0);
			}
		}
	}

	@Override
	public void doSecondLayoutPass(double parentsWidthFromFirstPass, double parentsHeightFromFirstPass) {
		name.setNonSize(parentsWidthFromFirstPass - number.getNonPosition().getWidth() - tag.getNonPosition().getWidth(), name
				.getNonPosition().getHeight());
		setNonSize(parentsWidthFromFirstPass, Math.ceil(getNonPosition().getHeight()));
	}

	@Override
	public void draw(Context2d ctx, int invalidationMask) {
		boolean selected = isSelected() || isContextMenuActiveOnMe();
		boolean loaded = isLoaded() && !isInStoreSelectMode();
		boolean isOriginPreset = isLoaded() && isInStoreSelectMode();

		if (isInMultiplePresetSelectionMode()) {
			selected = getParent().getParent().getMultiSelection().contains(this);
			loaded = false;
		}

		double cp = getConturPixels();
		cp = Math.ceil(cp);
		cp = Math.max(1, cp);

		RGB colorContour = new RGB(0, 0, 0);
		RGB colorFill = new RGB(25, 25, 25);
		RGB colorHighlight = getParent().getColorBankInnerBorder();

		if (filterSate == FilterState.FILTER_MATCHES) {

			colorFill = new RGB(50, 65, 110);

			if (getParent().getParent().isCurrentFilterMatch(this)) {
				colorContour = new RGB(230, 240, 255);
			}
		} else {
			if (loaded || isOriginPreset)
				colorFill = RGB.blue();
			else if (selected)
				colorFill = new RGB(77, 77, 77);
		}

		if (isDraggingControl())
			colorFill = colorFill.brighter(40);

		Rect r = getPixRect().copy();
		r.fill(ctx, colorFill);
		r.stroke(ctx, cp, colorHighlight);
		r.reduceHeightBy(2 * cp);
		r.reduceWidthBy(2 * cp);
		r.stroke(ctx, cp, colorContour);

		super.draw(ctx, invalidationMask);

		if (filterSate == FilterState.FILTERED_OUT) {
			r.fill(ctx, new RGBA(0, 0, 0, 0.75));
		} else if (filterSate == FilterState.FILTER_MATCHES) {

			if (getParent().getParent().isCurrentFilterMatch(this)) {
				r.stroke(ctx, 2 * cp, new RGB(230, 240, 255));
			}
		}
	}

	public boolean isSelected() {
		StoreSelectMode sm = NonMaps.get().getNonLinearWorld().getPresetManager().getStoreSelectMode();
		if (sm != null)
			return sm.getSelectedPreset() == this;

		return uuid.equals(getParent().getPresetList().getSelectedPreset());
	}

	public boolean isContextMenuActiveOnMe() {
		Overlay o = NonMaps.get().getNonLinearWorld().getViewport().getOverlay();
		ContextMenu c = o.getContextMenu();

		if (c instanceof PresetContextMenu) {
			PresetContextMenu p = (PresetContextMenu) c;
			if (p.getPreset() == this)
				return true;
		}
		return false;
	}

	public boolean isLoaded() {
		if (getParent().isInStoreSelectMode()) {
			return this == getParent().getParent().getStoreSelectMode().getOriginalPreset();
		}

		return uuid.equals(getNonMaps().getNonLinearWorld().getParameterEditor().getLoadedPresetUUID());
	}

	public boolean isInStoreSelectMode() {
		return NonMaps.get().getNonLinearWorld().getPresetManager().isInStoreSelectMode();
	}

	@Override
	public Control click(Position point) {
		if (isInMultiplePresetSelectionMode()) {
			getParent().getParent().getMultiSelection().toggle(this);
			invalidate(INVALIDATION_FLAG_UI_CHANGED);
		} else if (NonMaps.get().getNonLinearWorld().isShiftDown()) {
			getParent().getParent().startMultiSelection(this, true);
			invalidate(INVALIDATION_FLAG_UI_CHANGED);
		} else if (isInStoreSelectMode() || !isSelected()) {
			selectPreset();
		} else if (isSelected()) {
			load();
		}
		return this;
	}

	@Override
	public Control onContextMenu(Position pos) {
		if (isInStoreSelectMode())
			return null;

		ContextMenusSetting contextMenuSettings = NonMaps.theMaps.getNonLinearWorld().getViewport().getOverlay().getSetup()
				.getContextMenuSettings();
		if (contextMenuSettings.isEnabled()) {
			Overlay o = NonMaps.theMaps.getNonLinearWorld().getViewport().getOverlay();

			boolean isInMultiSel = isSelectedInMultiplePresetSelectionMode();

			if (isInMultiSel || (!isInMultiSel && !isInMultiplePresetSelectionMode()))
				return o.setContextMenu(pos, new PresetContextMenu(o, this));
		}
		return this;
	}

	public void selectPreset() {
		StoreSelectMode storeMode = getNonMaps().getNonLinearWorld().getPresetManager().getStoreSelectMode();
		if (storeMode != null) {
			storeMode.setSelectedPreset(this);
		} else {
			getParent().getPresetList().selectPreset(getUUID(), Initiator.EXPLICIT_USER_ACTION);

		}
		invalidate(INVALIDATION_FLAG_UI_CHANGED);
	}

	private boolean isSelectedInMultiplePresetSelectionMode() {
		MultiplePresetSelection mp = getParent().getParent().getMultiSelection();
		if (mp != null) {
			return mp.getSelectedPresets().contains(this.getUUID());
		}
		return false;
	}

	private boolean isInMultiplePresetSelectionMode() {
		return getParent().getParent().hasMultiplePresetSelection();
	}

	@Override
	public Control mouseDown(Position eventPoint) {
		getParent().getParent().pushBankOntoTop(getParent());
		return this;
	}

	@Override
	public Control startDragging(Position pos) {
		if (getNonMaps().getNonLinearWorld().getViewport().getOverlay().getSetup().getPresetDragDropSetting().isEnabled()) {
			if (isInMultiplePresetSelectionMode()) {
				return startMultipleSelectionDrag(pos);
			}

			return getNonMaps().getNonLinearWorld().getViewport().getOverlay().createDragProxy(this);
		}

		return super.startDragging(pos);
	}

	public Control startMultipleSelectionDrag(Position mousePos) {
		NonLinearWorld world = getNonMaps().getNonLinearWorld();
		PresetManager pm = world.getPresetManager();
		MultiplePresetSelection selection = pm.getMultiSelection();
		selection.add(this);

		Control ret = null;
		double yMargin = getPixRect().getTop() - mousePos.getY();
		double xMargin = getPixRect().getLeft() - mousePos.getX();

		Tracer.log("yMargin: " + yMargin);
		Tracer.log("xMargin: " + xMargin);

		for (String uuid : selection.getSelectedPresets()) {
			Preset p = pm.findPreset(uuid);
			if (p != null) {
				DragProxy a = world.getViewport().getOverlay().addDragProxy(p);
				if (p == this)
					ret = a;

				double xDiff = mousePos.getX() - p.getPixRect().getLeft();
				double yDiff = mousePos.getY() - p.getPixRect().getTop();
				a.animatePositionOffset(xDiff + xMargin, yDiff + yMargin);
				yMargin += getPixRect().getHeight();
			}
		}

		return ret;
	}

	@Override
	public Control mouseDrag(Position oldPoint, Position newPoint, boolean fine) {
		return this;
	}

	@Override
	public String getCurrentName() {
		return name.getText();
	}

	@Override
	public String getEntityName() {
		return "Preset";
	}

	@Override
	public void setName(String newName) {
		getNonMaps().getServerProxy().renamePreset(getUUID(), newName);
	}

	@Override
	public String getUUID() {
		return uuid;
	}

	@Override
	public RGB getColorObjectBackgroundSelected() {
		return new RGB(50, 50, 50);
	}

	@Override
	public double getXMargin() {
		return 3;
	}

	public void select() {
		getParent().getPresetList().selectPreset(getUUID(), Initiator.EXPLICIT_USER_ACTION);
	}

	public void load() {
		getNonMaps().getServerProxy().loadPreset(this);
	}

	public void setFilterState(FilterState state) {
		if (filterSate != state) {
			filterSate = state;
			invalidate(INVALIDATION_FLAG_UI_CHANGED);
		}
	}

	public FilterState getFilterState() {
		return filterSate;
	}

	private void updateAttributes(Node node) {
		if (ServerProxy.didChange(node)) {
			attributes.clear();
			NodeList children = node.getChildNodes();

			for (int i = 0; i < children.getLength(); i++) {
				Node n = children.item(i);
				String nodesName = n.getNodeName();

				if (nodesName.equals("attribute")) {
					updateAttribute(n);
				}
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

	@Override
	public String getTitleName() {
		return getCurrentName();
	}

	@Override
	public void beingDropped() {
		super.beingDropped();

		if (isInMultiplePresetSelectionMode())
			getParent().getParent().closeMultiSelection();
	}

	public void rename() {
		RenameDialog.open(this);
	}

}
