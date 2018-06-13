package com.nonlinearlabs.NonMaps.client.world.maps.parameters;

import java.util.HashSet;

import com.nonlinearlabs.NonMaps.client.Animator;
import com.nonlinearlabs.NonMaps.client.Animator.DoubleClientData.Client;
import com.nonlinearlabs.NonMaps.client.NonMaps;
import com.nonlinearlabs.NonMaps.client.world.Rect;
import com.nonlinearlabs.NonMaps.client.world.maps.MapsLayout;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.value.QuantizedClippedValue;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.value.QuantizedClippedValue.ChangeListener;

abstract public class PhysicalControlParameter extends Parameter {

	private HashSet<ModulationRoutingParameter> targets = new HashSet<ModulationRoutingParameter>();

	public PhysicalControlParameter(MapsLayout parent) {
		super(parent);
		addChild(new LabelLarge(this, getName()));
		addChild(new SliderHorizontalWithHandle(this));
		addChild(new ValueDisplaySmall(this));
	}

	@Override
	protected QuantizedClippedValue createValue(ChangeListener changeListener) {
		return new QuantizedClippedValue(changeListener) {

			Animator returnTimer = null;
			boolean animating = false;

			@Override
			public IncrementalChanger startUserEdit(double pixPerRange) {
				if (returnTimer != null)
					cancelReturnTimer();

				return super.startUserEdit(pixPerRange);
			}

			private void cancelReturnTimer() {
				if (returnTimer != null) {
					Animator toCancel = returnTimer;
					returnTimer = null;
					toCancel.cancel();
				}
			}

			@Override
			protected void onClippedValueChanged(Initiator initiator, double oldClippedValue, double newClippedValue) {
				if (!animating)
					cancelReturnTimer();

				super.onClippedValueChanged(initiator, oldClippedValue, newClippedValue);
			}

			@Override
			protected void onEditingFinished() {
				super.onEditingFinished();

				cancelReturnTimer();

				if (getReturnMode() != ReturnMode.None) {
					returnTimer = new Animator();
					returnTimer.addSubAnimation(getQuantizedClipped(), 0, new Client() {

						@Override
						public void animate(double v) {
							if (returnTimer != null) {
								animating = true;
								setRawValue(Initiator.EXPLICIT_USER_ACTION, v);
								animating = false;
							}
						}
					});
				}
			}
		};
	}

	@Override
	public boolean shouldHaveHandleOnly() {
		return true;
	}

	@Override
	protected int getBackgroundRoundings() {
		return Rect.ROUNDING_NONE;
	}

	@Override
	public boolean dimHandleAtDefaultValue() {
		switch (getReturnMode()) {
		case Center:
			return true;

		case None:
			return false;

		case Zero:
			return true;

		}
		return super.dimHandleAtDefaultValue();
	}

	@Override
	public void onValueChanged(Initiator initiator, double diff) {
		super.onValueChanged(initiator, diff);

		if (initiator == Initiator.EXPLICIT_USER_ACTION)
			if (Math.abs(diff) > 0.0)
				for (ModulationRoutingParameter p : targets)
					if (getReturnMode() != ReturnMode.None)
						p.applyReturningModulation(Initiator.MODULATION, diff);
					else
						p.applyDirectModulation(Initiator.MODULATION, getValue().getQuantizedClipped());
	}

	public void addModulationRoutingParameter(ModulationRoutingParameter modulationRoutingParameter) {
		targets.add(modulationRoutingParameter);
	}

	public String getDisplayName() {
		
		int idWithLargestAmount = 0;
		double largestAmount = 0;
		boolean isNegative = false;

		for (ModulationRoutingParameter p : targets) {
			double amount = p.getValue().getQuantizedClipped();
			if (Math.abs(amount) > largestAmount) {
				idWithLargestAmount = p.getTargetParameterID();
				largestAmount = Math.abs(amount);
				isNegative = amount < 0;
			}
		}

		if (idWithLargestAmount == 0)
			return "Not assigned";

		Parameter p = (Parameter) NonMaps.theMaps.getNonLinearWorld().getParameterEditor().findSelectable(idWithLargestAmount);
		return (isNegative ? "-" : "") + p.getName().getLongName();
	}

	public enum ReturnMode {
		None, Center, Zero;

		public String toString() {
			switch (this) {
			case None:
				return "None";

			case Zero:
				return "Zero";

			case Center:
				return "Center";

			default:
				return "";
			}
		}

		public static ReturnMode fromInt(int i) {
			for (ReturnMode m : ReturnMode.values())
				if (m.ordinal() == i)
					return m;
			return ReturnMode.None;
		}
	}

	public abstract ReturnMode getReturnMode();

	@Override
	public String getGroupName() {
		return "Hardware Source";
	}
}