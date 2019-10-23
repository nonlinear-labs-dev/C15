package com.nonlinearlabs.client.world.maps.parameters.PlayControls.SourcesAndAmounts.Sources;

import com.google.gwt.xml.client.Node;
import com.nonlinearlabs.client.ServerProxy;
import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.parameters.ParameterEditor;
import com.nonlinearlabs.client.world.maps.parameters.PhysicalControlParameter;
import com.nonlinearlabs.client.world.maps.parameters.PlayControls.SourcesAndAmounts.SourcesAndAmounts;
import com.nonlinearlabs.client.world.overlay.Overlay;
import com.nonlinearlabs.client.world.overlay.belt.parameters.PedalContextMenu;

public class Pedal extends PhysicalControlParameter {

	private int currentMode = 0;

	Pedal(MapsLayout parent, int parameterID) {
		super(parent, parameterID);
	}

	@Override
	public PedalContextMenu createContextMenu(Overlay o) {
		return new PedalContextMenu(o, this);
	}

	@Override
	public boolean hasContextMenu() {
		return true;
	}

	@Override
	protected boolean updateValues(Node child) {
		if (super.updateValues(child))
			return true;

		String nodeName = child.getNodeName();

		String value = ServerProxy.getText(child);

		if (nodeName.equals("pedal-mode")) {
			int m = Integer.parseInt(value);
			if (m != currentMode) {
				currentMode = m;
				onPedalModeChanged();
			}
			return true;
		}
		return getName().update(child);
	}

	private void onPedalModeChanged() {
		ParameterEditor eddi = (ParameterEditor) getSelectionRoot();
		SourcesAndAmounts mappings = (SourcesAndAmounts) eddi.findParameterGroup("MCM");
		mappings.onReturningModeChanged(this);
		invalidate(INVALIDATION_FLAG_UI_CHANGED);
	}

	@Override
	public ReturnMode getReturnMode() {
		switch (currentMode) {
		case 0:
			return ReturnMode.None;

		case 1:
			return ReturnMode.Zero;

		case 2:
			return ReturnMode.Center;
		}
		return ReturnMode.None;
	}

}