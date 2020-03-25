package com.nonlinearlabs.client.world.pointer;

import java.util.ArrayList;

import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;

class TouchMove extends Drag {

	private ArrayList<Position> touches;

	TouchMove(Gesture predecessor, Position from, ArrayList<Touch> touches) {
		super(predecessor, from, new Position(touches.get(0)), false);
		this.touches = new ArrayList<Position>();

		for (int i = 0; i < touches.size(); i++) {
			Touch t = touches.get(i);
			this.touches.add(new Position(t));
		}
	}

	@Override
	public Gesture onTouchStart(ArrayList<Touch> touches) {
		return super.onTouchMove(touches);
	}

	@Override
	public Gesture onTouchMove(ArrayList<Touch> touches) {
		if (touches.size() == 1) {
			return new TouchMove(this, getPosition(), touches);
		} else if (touches.size() == 2) {
			return new TouchPinchStart(this, touches);
		}

		return this;

	}

	@Override
	public Gesture onTouchEnd(ArrayList<Touch> touches) {
		if (touches.size() > 0)
			return onTouchMove(touches);

		return new TouchMoveEnd(this);
	}

	@Override
	public Control applyTo(Control receiver) {
		HasPosition prev = (HasPosition) getPredecessor();
		Control ret = receiver.mouseDrag(prev.getPosition(), getPosition(), false);

		if (ret != null) {
			setReceiver(receiver);
		}

		return ret;
	}

	@Override
	public Position getPosition() {
		return touches.get(0);
	}

}
