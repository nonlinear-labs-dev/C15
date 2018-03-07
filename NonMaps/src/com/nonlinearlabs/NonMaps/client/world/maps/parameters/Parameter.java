package com.nonlinearlabs.NonMaps.client.world.maps.parameters;

import java.util.HashSet;

import com.google.gwt.canvas.dom.client.Context2d;
import com.google.gwt.core.client.JavaScriptObject;
import com.google.gwt.xml.client.Node;
import com.google.gwt.xml.client.NodeList;
import com.nonlinearlabs.NonMaps.client.Checksum;
import com.nonlinearlabs.NonMaps.client.Millimeter;
import com.nonlinearlabs.NonMaps.client.NonMaps;
import com.nonlinearlabs.NonMaps.client.ServerProxy;
import com.nonlinearlabs.NonMaps.client.Tracer;
import com.nonlinearlabs.NonMaps.client.world.Control;
import com.nonlinearlabs.NonMaps.client.world.Gray;
import com.nonlinearlabs.NonMaps.client.world.Name;
import com.nonlinearlabs.NonMaps.client.world.Position;
import com.nonlinearlabs.NonMaps.client.world.RGB;
import com.nonlinearlabs.NonMaps.client.world.Rect;
import com.nonlinearlabs.NonMaps.client.world.maps.LayoutResizingVertical;
import com.nonlinearlabs.NonMaps.client.world.maps.MapsControl;
import com.nonlinearlabs.NonMaps.client.world.maps.MapsLayout;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.value.QuantizedClippedValue;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.value.QuantizedClippedValue.ChangeListener;
import com.nonlinearlabs.NonMaps.client.world.overlay.ContextMenu;
import com.nonlinearlabs.NonMaps.client.world.overlay.Overlay;
import com.nonlinearlabs.NonMaps.client.world.overlay.setup.ContextMenusSetting;
import com.nonlinearlabs.NonMaps.client.world.overlay.setup.EditParameterSetting;
import com.nonlinearlabs.NonMaps.client.world.pointer.TouchPinch;

public abstract class Parameter extends LayoutResizingVertical {

	private class ValueChangeListener implements QuantizedClippedValue.ChangeListener {
		@Override
		public void onClippedValueChanged(Initiator initiator, double oldClippedValue, double newClippedValue) {
		}

		@Override
		public void onQuantizedValueChanged(Initiator initiator, double oldQuantizedValue, double newQuantizedValue) {
		}

		@Override
		public void onRawValueChanged(Initiator initiator, double oldRawValue, double newRawValue) {
			Parameter.this.onValueChanged(initiator, newRawValue - oldRawValue);
		}
	}

	public enum Initiator {
		EXPLICIT_USER_ACTION, INDIRECT_USER_ACTION, MODULATION
	}

	public interface ParameterListener {
		public void onParameterChanged(QuantizedClippedValue newValue);
	}

	private HashSet<ParameterListener> listeners = new HashSet<ParameterListener>();
	private QuantizedClippedValue value;
	private JavaScriptObject stringizer;
	private Name name;
	private boolean isLocked = false;
	protected QuantizedClippedValue.IncrementalChanger currentParameterChanger = null;

	public Parameter(MapsLayout parent) {
		super(parent);
		name = createName();
		value = createValue(new ValueChangeListener());

		if (getParameterID() != 0)
			getSelectionRoot().registerSelectable(this);
	}

	protected QuantizedClippedValue createValue(ChangeListener changeListener) {
		return new QuantizedClippedValue(changeListener);
	}

	protected Name createName() {
		return new Name();
	}

	@Override
	public void getStateHash(Checksum crc) {
		super.getStateHash(crc);
		crc.eat(isSelected());
		crc.eat(getParameterID());
	}

	public final boolean isBiPolar() {
		return getValue().isBipolar();
	}

	public abstract int getParameterID();

	@Override
	public void draw(Context2d ctx, int invalidationMask) {

		getPixRect().drawRoundedRect(ctx, getBackgroundRoundings(), toXPixels(4), 0, getParameterBackgroundColor(), null);

		super.draw(ctx, invalidationMask);

		if (isSelected())
			getPixRect().drawRoundedRect(ctx, getBackgroundRoundings(), toXPixels(4), toXPixels(1), null, getColorSliderHighlight());
	}

	private RGB getParameterBackgroundColor() {
		return isSelected() ? getColorObjectBackgroundSelected() : new Gray(52);
	}

	protected int getBackgroundRoundings() {
		int numChildrenInParent = getParent().getChildren().size();

		if (numChildrenInParent == 1)
			return Rect.ROUNDING_ALL;

		if (getParent().getChildren().get(0) == this)
			return Rect.ROUNDING_TOP;

		if (getParent().getChildren().get(numChildrenInParent - 1) == this)
			return Rect.ROUNDING_BOTTOM;

		return Rect.ROUNDING_NONE;
	}

	public void setDefault(Initiator initiator) {
		getValue().setToDefault(initiator);
	}

	public void select(Initiator initiator) {
		getSelectionRoot().select(initiator, this);
	}

	public boolean isSelected() {
		return getSelectionRoot().getSelection() == this;
	}

	public Name getName() {
		return name;
	}

	@Override
	public Control click(Position eventPoint) {
		if(getWorld().isSpaceDown())
			return this;
		
		boolean isAnyChildVisible = isAnyChildVisible();

		if (!isAnyChildVisible)
			return null;

		if (!isSelected())
			select(Initiator.EXPLICIT_USER_ACTION);

		return this;
	}

	@Override
	public Control doubleClick() {
		setDefault(Initiator.EXPLICIT_USER_ACTION);
		return this;
	}

	@Override
	public Control mouseDown(Position eventPoint) {
		if(getWorld().isSpaceDown())
			return this;
		
		EditParameterSetting s = getWorld().getViewport().getOverlay().getSetup().getEditParameterDragSetting();
		String choiceString = s.getSettingsControl().getChoiceString();

		switch (choiceString) {
		case "Always":
			select(Initiator.EXPLICIT_USER_ACTION);

			if (isBoolean())
				toggleBoolean();
			else
				currentParameterChanger = getValue().startUserEdit(Millimeter.toPixels(100));
			return this;

		case "If Selected":
			if (isSelected()) {
				if (isBoolean())
					toggleBoolean();
				else
					currentParameterChanger = getValue().startUserEdit(Millimeter.toPixels(100));
				return this;
			}

			break;

		case "Never":
			break;

		default:
			break;
		}

		return super.mouseDown(eventPoint);
	}

	@Override
	public boolean wantsMouseUpAnimation() {
		return false;
	}

	@Override
	public Control pinch(Position eventPoint, double touchDist, TouchPinch pinch) {
		if (isSelected()) {
			Tracer.log("Parameter.pinch");
			return NonMaps.theMaps.getNonLinearWorld();
		}
		return super.pinch(eventPoint, touchDist, pinch);
	}

	@Override
	public Control mouseDrag(Position oldPoint, Position newPoint, boolean fine) {
		EditParameterSetting s = getWorld().getViewport().getOverlay().getSetup().getEditParameterDragSetting();
		boolean noDrag = s.getSettingsControl().getChoiceString().equals("Never") || getWorld().isSpaceDown();

		if (isSelected() && !noDrag) {

			double xPix = newPoint.getX() - oldPoint.getX();
			double yPix = oldPoint.getY() - newPoint.getY();
			double pix = xPix;

			if (Math.abs(yPix) > Math.abs(xPix))
				pix = yPix;

			if (currentParameterChanger != null)
				currentParameterChanger.changeBy(fine, pix);

			return this;
		} else if (noDrag) {
			return getWorld().mouseDrag(oldPoint, newPoint, fine);
		}

		return null;
	}

	@Override
	public void onMouseLost() {
		if (currentParameterChanger != null) {
			currentParameterChanger.finish();
			currentParameterChanger = null;
		}
		super.onMouseLost();
	}

	private boolean isAnyChildVisible() {
		boolean isAnyChildVisible = false;

		for (MapsControl c : getChildren()) {
			isAnyChildVisible |= c.isVisible();

			if (isAnyChildVisible)
				break;
		}
		return isAnyChildVisible;
	}

	protected ParameterEditor getSelectionRoot() {
		return ParameterEditor.get();
	}

	public QuantizedClippedValue getValue() {
		return value;
	}

	public String getDecoratedValue(boolean withUnit) {
		return getDecoratedValue(withUnit, value.getQuantizedClipped());
	}

	public String getDecoratedValue(boolean withUnit, double cpValue) {
		if (stringizer == null)
			return "";

		return stringize(withUnit, cpValue);
	}

	public void setStringizer(String txt) {
		if (stringizer == null)
			createStringizer(txt);
	}

	private native void createStringizer(String body) /*-{
														this.@com.nonlinearlabs.NonMaps.client.world.maps.parameters.Parameter::stringizer = new Function(
														"cpValue", "withUnit", body);
														}-*/;

	private native String stringize(boolean withUnit, double cpValue) /*-{
																		var stringizer = this.@com.nonlinearlabs.NonMaps.client.world.maps.parameters.Parameter::stringizer;
																		var scaledText = stringizer(cpValue, withUnit);
																		return scaledText;
																		}-*/;

	public void addListener(ParameterListener l) {
		listeners.add(l);
	}

	public void removeListener(ParameterListener l) {
		listeners.remove(l);
	}

	protected void notifyListeners() {
		for (ParameterListener l : listeners)
			l.onParameterChanged(value);
	}

	void update(Node n) {
		NodeList paramChildren = n.getChildNodes();
		String locked = n.getAttributes().getNamedItem("locked").getNodeValue();
		boolean isLocked = locked.equals("1");

		if (isLocked != this.isLocked) {
			this.isLocked = isLocked;
			invalidate(INVALIDATION_FLAG_UI_CHANGED);
		}

		for (int j = 0; j < paramChildren.getLength(); j++) {
			Node child = paramChildren.item(j);
			child.normalize();
			String nodeName = child.getNodeName();

			if (!nodeName.isEmpty() && !nodeName.startsWith("#"))
				updateValues(child);
		}
	}

	protected boolean updateValues(Node child) {
		getValue().update(child);

		String nodeName = child.getNodeName();

		try {
			String value = ServerProxy.getText(child);

			if (nodeName.equals("scaling")) {
				updateStringizer(child, value);
				return true;
			}
		} catch (Exception e) {
		}
		return getName().update(child);
	}

	private void updateStringizer(Node child, String value) {
		setStringizer(value);
	}

	private void updateValue(Node child, String value, Initiator initiator) {
		getValue().setRawValue(initiator, Double.parseDouble(value));
	}

	public void inc(boolean fine) {
		getValue().inc(Initiator.EXPLICIT_USER_ACTION, fine);
	}

	public void dec(boolean fine) {
		getValue().dec(Initiator.EXPLICIT_USER_ACTION, fine);
	}

	public void onValueChanged(Initiator initiator, double diff) {
		if (initiator == Initiator.EXPLICIT_USER_ACTION)
			getNonMaps().getServerProxy().onParameterChanged(Parameter.this);

		notifyListeners();
		invalidate(INVALIDATION_FLAG_UI_CHANGED);
	}

	public boolean isBoolean() {
		return getValue().isBoolean();
	}

	private void toggleBoolean() {
		if (getValue().getQuantizedClipped() != 0.0)
			getValue().setRawValue(Initiator.EXPLICIT_USER_ACTION, 0.0);
		else
			getValue().setRawValue(Initiator.EXPLICIT_USER_ACTION, 1.0);

		invalidate(INVALIDATION_FLAG_UI_CHANGED);
	}

	public boolean shouldHaveHandleOnly() {
		return false;
	}

	public ContextMenu createContextMenu(Overlay o) {
		return null;
	}

	public boolean hasContextMenu() {
		return false;
	}

	public boolean dimHandleAtDefaultValue() {
		return false;
	}

	public boolean isOracle() {
		return true;
	}

	public static Parameter findInParents(MapsControl c) {
		if (c instanceof Parameter)
			return (Parameter) c;

		return findInParents(c.getParent());
	}

	public MapsLayout getParameterGroup() {
		Control p = getParent();
		while (p != null) {
			if (p instanceof ParameterGroupIface)
				return (MapsLayout) p;

			p = p.getParent();
		}
		return null;
	}

	@Override
	public Control onContextMenu(Position pos) {

		select(Initiator.EXPLICIT_USER_ACTION);

		ContextMenusSetting contextMenuSettings = NonMaps.theMaps.getNonLinearWorld().getViewport().getOverlay().getSetup()
				.getContextMenuSettings();
		if (contextMenuSettings.isEnabled()) {
			if (hasContextMenu()) {
				Overlay o = NonMaps.theMaps.getNonLinearWorld().getViewport().getOverlay();
				return o.setContextMenu(pos, createContextMenu(o));
			}
		}
		return super.onContextMenu(pos);
	}

	public String getFullNameWithGroup() {
		return getGroupName() + "   \u2013   " + getName().getLongName();
	}

	public String getGroupName() {
		ParameterGroupIface group = (ParameterGroupIface) getParameterGroup();
		return group.getName().getLongName();
	}

	public boolean isLocked() {
		return isLocked;
	}

}
