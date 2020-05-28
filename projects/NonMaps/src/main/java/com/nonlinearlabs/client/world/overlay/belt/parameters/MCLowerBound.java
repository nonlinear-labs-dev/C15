package com.nonlinearlabs.client.world.overlay.belt.parameters;

import com.nonlinearlabs.client.dataModel.editBuffer.ParameterId;
import com.nonlinearlabs.client.presenters.EditBufferPresenterProvider;
import com.nonlinearlabs.client.presenters.ParameterPresenter;
import com.nonlinearlabs.client.useCases.EditBufferUseCases;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.maps.parameters.value.QuantizedClippedValue;

public class MCLowerBound extends MCSomething {

	QuantizedClippedValue dummyValue;

	public MCLowerBound(Control parent) {
		super(parent);
		addChild(middle = new MCLowerBoundLabel(this));
	}

	@Override
	public MCLowerBoundLabel getMiddle() {
		return (MCLowerBoundLabel) super.getMiddle();
	}

	@Override
	public Control mouseDown(Position eventPoint) {
		ParameterPresenter p = EditBufferPresenterProvider.getPresenter().selectedParameter;

		if (p.modulation.isModulated)
			getParent().startEdit(BeltParameterLayout.Mode.mcLower, getPixRect().getWidth());

		return getParent();
	}

	@Override
	public Control doubleClick(Position pos) {
		ParameterPresenter p = EditBufferPresenterProvider.getPresenter().selectedParameter;

		if (p.modulation.isModulated)
			EditBufferUseCases.get().startEditModulationAmountLowerBound(p.id, 100).setToDefault();

		return this;
	}

	@Override
	protected void inc(ParameterId id, boolean fine) {
		EditBufferUseCases.get().startEditModulationAmountLowerBound(id, 100).inc(fine);
	}

	@Override
	protected void dec(ParameterId id, boolean fine) {
		EditBufferUseCases.get().startEditModulationAmountLowerBound(id, 100).dec(fine);
	}
}
