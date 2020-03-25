package com.nonlinearlabs.client.world.pointer;

import java.util.ArrayList;

import com.google.gwt.user.client.Timer;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;

class TouchMoveStart extends TouchMove {

	private Timer longPressTimer;

	TouchMoveStart(Gesture predecessor, Position from, ArrayList<Touch> touches) {
		super(predecessor, from, touches);

		NonMaps.theMaps.captureMouse();

		longPressTimer = new Timer() {
			public void run() {
				PointerState.get().longPressTimerExpired(TouchMoveStart.this);
			}
		};
		longPressTimer.schedule(500);
	}

	@Override
	public Gesture onTouchMove(ArrayList<Touch> touches) {
		return new TouchMove(this, getPosition(), touches);
	}

	public Gesture onLongPress() {
		return new TouchLongPress(this);
	}

	@Override
	public Gesture onTouchEnd(ArrayList<Touch> touches) {
		if (touches.size() > 0)
			return onTouchMove(touches);

		return new TouchMoveEnd(this);
	}

	@Override
	public Control applyTo(Control receiver) {
		Control ret = receiver.startDragging(getStartPosition());

		if (ret != null) {
			setReceiver(receiver);
			return super.applyTo(ret);
		}

		return ret;
	}
}
