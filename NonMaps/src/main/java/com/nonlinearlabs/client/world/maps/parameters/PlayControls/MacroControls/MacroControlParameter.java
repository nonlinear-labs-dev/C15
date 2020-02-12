package com.nonlinearlabs.client.world.maps.parameters.PlayControls.MacroControls;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.ColorTable;
import com.nonlinearlabs.client.Renameable;
import com.nonlinearlabs.client.dataModel.editBuffer.ParameterId;
import com.nonlinearlabs.client.useCases.EditBufferUseCases;
import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.parameters.Parameter;
import com.nonlinearlabs.client.world.maps.parameters.SliderHorizontalWithHandle;
import com.nonlinearlabs.client.world.maps.parameters.UnModulateableParameterName;
import com.nonlinearlabs.client.world.maps.parameters.ValueDisplaySmall;
import com.nonlinearlabs.client.world.overlay.ContextMenu;
import com.nonlinearlabs.client.world.overlay.Overlay;
import com.nonlinearlabs.client.world.overlay.ParameterInfoDialog;
import com.nonlinearlabs.client.world.overlay.belt.parameters.MacroControlContextMenu;

public abstract class MacroControlParameter extends Parameter implements Renameable {

	private String info = "";

	private class MacroControlValueDisplay extends ValueDisplaySmall {
		private MacroControlValueDisplay(MapsLayout parent, int parameterID) {
			super(parent, parameterID);
		}

		@Override
		protected double getBasicHeight() {
			return 22;
		}
	}

	private class MacroControlSlider extends SliderHorizontalWithHandle {
		private MacroControlSlider(MapsLayout parent, int parameterID) {
			super(parent, parameterID);
		}
	}

	MacroControlParameter(MacrosCol parent, String defName, int parameterID) {
		super(parent, parameterID);
		addChild(new UnModulateableParameterName(this) {
			@Override
			protected double getBasicHeight() {
				return 23;
			}

			@Override
			protected double getBasicWidth() {
				return 80;
			}

			@Override
			protected String getDisplayText() {
				MacroControlParameter mc = (MacroControlParameter)(getParent());
				if(mc != null)
				{
					return mc.getTitleName();
				}
				return super.getDisplayText();
			}

		});
		addChild(new MacroControlSlider(this, getParameterNumber()));
		addChild(new MacroControlValueDisplay(this, getParameterNumber()));
	}

	@Override
	public void draw(Context2d ctx, int invalidationMask) {
		super.draw(ctx, invalidationMask);

		if (isSelected())
			getPixRect().drawRoundedRect(ctx, getBackgroundRoundings(), toXPixels(4), toXPixels(1), null,
					ColorTable.getMacroControlTargetColor());
	}

	@Override
	public String getCurrentName() {
		return presenter.userGivenName;
	}

	@Override
	public String getTitleName() {
		return presenter.longName;
	}

	@Override
	public String getEntityName() {
		return "Macro Control";
	}

	@Override
	public void setName(String newName) {
		ParameterId id = EditBufferUseCases.get().toParamId(getParameterNumber());
		EditBufferUseCases.get().renameMacroControl(id, newName);
	}

	@Override
	public ContextMenu createContextMenu(Overlay o) {
		return new MacroControlContextMenu(o, this);
	}

	public void setInfo(String text) {
		if (!text.equals(info)) {
			info = text;
			if (ParameterInfoDialog.isShown())
				ParameterInfoDialog.update();
		}
	}

	public String getInfo() {
		return info;
	}
}