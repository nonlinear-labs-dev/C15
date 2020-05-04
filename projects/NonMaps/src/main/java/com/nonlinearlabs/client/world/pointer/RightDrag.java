package com.nonlinearlabs.client.world.pointer;

import com.nonlinearlabs.client.world.Position;

class RightDrag extends Drag {

	RightDrag(Gesture predecessor, Position from, Position to, boolean fine) {
		super(predecessor, from, to, fine);
	}

	@Override
	public Gesture onRightUp() {
		return new RightUp(this);
	}

	@Override
	public Gesture move(boolean fine) {
		return new RightDrag(this, getPosition(), PointerState.get().getPosition(), fine);
	}

}
