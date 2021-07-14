package com.nonlinearlabs.client.world.pointer;

import java.util.ArrayList;

import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.pointer.Gesture.HasPosition;

public class TouchPinch extends Gesture implements HasPosition, Gesture.NeedsFocus {

	private ArrayList<Position> touches;

	TouchPinch(Gesture predecessor, ArrayList<Touch> touches) {
		super(predecessor);
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
			return new TouchMoveStart(this, getPosition(), touches);
		} else if (touches.size() == 2) {
			return new TouchPinch(this, touches);
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

		double distanceDiff = 0;

		if (touches.size() > 1) {
			double distance = getDistance();

			if (getPredecessor() instanceof TouchPinch) {
				TouchPinch p = (TouchPinch) getPredecessor();

				double formerDistance = p.getDistance();

				if (formerDistance > 0 && distance > 0)
					distanceDiff = distance - formerDistance;
			}
		}
		Control ret = receiver.pinch(getPosition(), distanceDiff, this);

		if (ret != null)
			setReceiver(receiver);

		return ret;
	}

	private double getDistance() {
		if (touches.size() > 1) {
			Position a = touches.get(0);
			Position b = touches.get(1);
			return a.getDistanceTo(b);
		}
		return 0;
	}

	@Override
	public Position getPosition() {
		if (touches.size() == 2) {
			Position a = touches.get(0);
			Position b = touches.get(1);
			return a.getMiddle(b);
		}

		return touches.get(0);
	}

	public ArrayList<Position> getTouches() {
		return touches;
	}

	public double getMaxTouchDistance() {
		double max = 0;

		if (getPredecessor() instanceof TouchPinch) {
			TouchPinch p = (TouchPinch) getPredecessor();

			int numTouches = Math.min(touches.size(), p.touches.size());

			for (int i = 0; i < numTouches; i++) {
				double diff = touches.get(i).getX() - p.touches.get(i).getX();
				if (Math.abs(diff) > Math.abs(max))
					max = diff;
			}
		}
		return max;
	}
}
