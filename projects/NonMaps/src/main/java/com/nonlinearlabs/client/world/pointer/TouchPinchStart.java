package com.nonlinearlabs.client.world.pointer;

import java.util.ArrayList;

import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.Tracer;
import com.nonlinearlabs.client.world.Control;

class TouchPinchStart extends TouchPinch {

	TouchPinchStart(Gesture predecessor, ArrayList<Touch> touches) {
		super(predecessor, touches);
		NonMaps.theMaps.captureMouse();
	}

	@Override
	public Gesture onTouchMove(ArrayList<Touch> touches) {
		return new TouchPinch(this, touches);
	}

	@Override
	public Gesture onTouchEnd(ArrayList<Touch> touches) {
		if (touches.size() > 0)
			return onTouchMove(touches);

		return new TouchPinchEnd(this);
	}

	@Override
	public Control applyTo(Control receiver) {
		Tracer.log("TouchPinchStart.applyTo -- 1");
		Control ret = receiver.pinch(getPosition(), 0, this);

		if (ret != null)
			setReceiver(receiver);

		Tracer.log("TouchPinchStart.applyTo -- 2 - " + ret);
		return ret;
	}

}
