package com.nonlinearlabs.client.world.pointer;

import java.util.ArrayList;

import com.nonlinearlabs.client.world.Control;

class TouchLongPress extends LongPress {

	TouchLongPress(Gesture predecessor) {
		super(predecessor);
	}

	@Override
	public Gesture onTouchEnd(ArrayList<Touch> touches) {
		if (touches.size() > 0)
			return onTouchMove(touches);

		return new TouchMoveEnd(this);
	}

	@Override
	public Gesture onTouchMove(ArrayList<Touch> touches) {
		if (getDistance(getPosition(), PointerState.get().getPosition()) > getDragHysteresis())
			return new TouchMoveStart(this, getPosition(), touches);

		return this;
	}

	@Override
	public Control applyTo(Control receiver) {
		return null;
	}

}
