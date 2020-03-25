package com.nonlinearlabs.client.world.pointer;

import java.util.ArrayList;

import com.nonlinearlabs.client.Millimeter;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;

class TouchDown extends Down {

	private ArrayList<Touch> touches = new ArrayList<Touch>();

	TouchDown(ArrayList<Touch> touches) {
		touches.forEach(a -> {
			this.touches.add(new Touch(a.id, a.pos));
		});
	}

	@Override
	public Gesture onTouchEnd(ArrayList<Touch> touches) {
		if (touches.size() > 0)
			return onTouchMove(touches);

		return new TouchUp(this);
	}

	@Override
	public Gesture onTouchMove(ArrayList<Touch> touches) {
		if (touches.size() == 1) {
			if (getDistance(this.touches.get(0).pos, touches.get(0).pos) > Millimeter.toPixels(5)) {
				Position myPos = this.touches.get(0).pos;
				ArrayList<Touch> newTouches = touches;
				return new TouchMoveStart(this, myPos, newTouches);
			}

			return this;
		} else if (touches.size() == 2) {
			return new TouchPinchStart(this, touches);
		}
		return this;
	}

	@Override
	public Control applyTo(Control receiver) {
		if (touches.size() == 1)
			return super.applyTo(receiver);

		return null;
	}

	@Override
	public Gesture onLongPress() {
		return new TouchLongPress(this);
	}
}
