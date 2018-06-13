package com.nonlinearlabs.NonMaps.client.world.maps.parameters;

import com.google.gwt.core.client.Scheduler;
import com.google.gwt.core.client.Scheduler.ScheduledCommand;
import com.nonlinearlabs.NonMaps.client.NonMaps;
import com.nonlinearlabs.NonMaps.client.world.Rect;
import com.nonlinearlabs.NonMaps.client.world.maps.MapsLayout;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.PhysicalControlParameter.ReturnMode;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.PlayControls.MacroControls.MacroControlParameter;

public class ModulationRoutingParameter extends Parameter {

	private int paramID;
	private int targetParamID;
	private int sourceParamID;

	public ModulationRoutingParameter(MapsLayout parent, int paramID, final int srcParamID, int targetParamID) {
		super(parent);

		this.paramID = paramID;
		this.sourceParamID = srcParamID;
		this.targetParamID = targetParamID;

		getSelectionRoot().registerSelectable(this);

		Scheduler.get().scheduleFinally(new ScheduledCommand() {

			@Override
			public void execute() {
				registerListener(srcParamID);
			}
		});

		ParameterEditor eddi = (ParameterEditor) getSelectionRoot();
		onReturningModeChanged((PhysicalControlParameter) eddi.findSelectable(sourceParamID));
	}

	@Override
	protected int getBackgroundRoundings() {
		return Rect.ROUNDING_NONE;
	}

	protected void setupChildren(boolean returning) {
		removeAll();

		if (returning) {

			addChild(new Spacer(this) {
				@Override
				protected double getBasicHeight() {
					return 20;
				}

				@Override
				protected double getMinHeight() {
					return 10;
				}

				@Override
				public double getLevelOfDetailForFullVisibility() {
					return getParent().getLevelOfDetailForFullVisibility() + 1;
				}
			});

			addChild(new SliderHorizontal(this) {
				@Override
				protected double getBasicWidth() {
					return 120;
				}

				@Override
				protected double getBasicHeight() {
					return 9;
				}
			});

			addChild(new ValueDisplaySmall(this));
		} else {
			addChild(createRoutingButton());
		}

		requestLayout();
	}

	protected ModulationRoutingButton createRoutingButton() {
		return new ModulationRoutingButton(this);
	}

	private void registerListener(int srcParamID) {
		ParameterEditor eddi = (ParameterEditor) getSelectionRoot();
		PhysicalControlParameter src = (PhysicalControlParameter) eddi.findSelectable(srcParamID);
		src.addModulationRoutingParameter(this);
	}

	@Override
	public int getParameterID() {
		return paramID;
	}

	public int getTargetParameterID() {
		return targetParamID;
	}

	void applyReturningModulation(Initiator initiator, double diff) {
		ParameterEditor eddi = (ParameterEditor) getSelectionRoot();
		MacroControlParameter tgt = (MacroControlParameter) eddi.findSelectable(targetParamID);
		double amount = getValue().getQuantizedClipped();

		if (amount != 0.0)
			tgt.applyModulation(initiator, diff * amount);
	}

	public void applyDirectModulation(Initiator initiator, double value) {
		ParameterEditor eddi = (ParameterEditor) getSelectionRoot();
		MacroControlParameter tgt = (MacroControlParameter) eddi.findSelectable(targetParamID);
		double amount = getValue().getQuantizedClipped();

		if (amount != 0.0)
			tgt.getValue().setRawValue(initiator, value);
	}

	public void onReturningModeChanged(PhysicalControlParameter src) {
		if (src.getParameterID() == sourceParamID) {
			setupChildren(src.getReturnMode() != ReturnMode.None);
			getValue().setBoolean(src.getReturnMode() == ReturnMode.None);
		}
	}

	public PhysicalControlParameter getSource() {
		return (PhysicalControlParameter) NonMaps.get().getNonLinearWorld().getParameterEditor().findParameter(sourceParamID);
	}

}