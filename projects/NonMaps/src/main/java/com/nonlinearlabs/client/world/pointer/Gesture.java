package com.nonlinearlabs.client.world.pointer;

import java.util.ArrayList;

import com.nonlinearlabs.client.Millimeter;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;

public abstract class Gesture {

	Control receiver;

	static double getDragHysteresis() {
		return Millimeter.toPixels(1.5);
	}

	interface NeedsFocus {
	}

	public interface HasPosition {
		Position getPosition();
	}

	interface GestureInspector {
		public boolean onGestureFound(Gesture g);
	}

	private Gesture predecessor;

	Gesture(Gesture predecessor) {
		this.predecessor = predecessor;
	}

	public Gesture getPredecessor() {
		return predecessor;
	}

	public Control getReceiver() {
		return receiver;
	}

	Gesture recursePredecessors(GestureInspector inspector) {
		if (predecessor != null) {
			if (inspector.onGestureFound(predecessor))
				return predecessor;

			return predecessor.recursePredecessors(inspector);
		}
		return null;
	}

	Gesture wheel(int delta, boolean fine) {
		return new Wheel(delta, fine);
	}

	public Gesture onLeftDown(boolean ctrl) {
		return impossible();
	}

	public Gesture onLeftUp() {
		return impossible();
	}

	public Gesture onLeftDoubleClick() {
		return impossible();
	}

	public Gesture onRightDown() {
		return impossible();
	}

	public Gesture onRightUp() {
		return impossible();
	}

	public Gesture onRightDoubleClick() {
		return impossible();
	}

	public Gesture onTouchStart(ArrayList<Touch> touches) {
		return impossible();
	}

	public Gesture onTouchMove(ArrayList<Touch> touches) {
		return impossible();
	}

	public Gesture onTouchEnd(ArrayList<Touch> touches) {
		return impossible();
	}

	public Gesture move(boolean fine) {
		return impossible();
	}

	public Gesture onLongPress() {
		return impossible();
	}

	private Gesture impossible() {
		return this;
	}

	public abstract Control applyTo(Control receiver);

	protected void setReceiver(Control receiver) {
		this.receiver = receiver;
	}

	public Gesture getFollower() {
		return null;
	}

	public boolean isTerminus() {
		return false;
	}

	double getDistance(Position a, Position b) {
		double diffX = a.getX() - b.getX();
		double diffY = a.getY() - b.getY();
		return Math.sqrt(diffX * diffX + diffY * diffY);
	}
}
