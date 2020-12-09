package com.nonlinearlabs.client.world.pointer;

import java.util.ArrayList;

import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;

public class PointerState {
	private static PointerState theState = new PointerState();

	private Position position = new Position(0, 0);
	private Gesture currentGesture = new Identity();
	private Control currentReceiver = null;

	static public PointerState get() {
		return theState;
	}

	public boolean isCurrentReceiver(Control q) {
		return q == currentReceiver;
	}

	public Position getPosition() {
		return position;
	}

	public void onMove(Position p, boolean fine) {
		this.position = p;
		setGesture(currentGesture.move(fine));
	}

	public void onWheel(Position p, int deltaY, boolean fine) {
		this.position = p;
		setGesture(currentGesture.wheel(deltaY, fine));
	}

	public void onLeftDown(Position position, boolean ctrl) {
		this.position = position;
		setGesture(currentGesture.onLeftDown(ctrl));
	}

	public void onLeftUp(Position position) {
		this.position = position;
		setGesture(currentGesture.onLeftUp());
	}

	public void onLeftDoubleClick(Position position) {
		this.position = position;
		setGesture(currentGesture.onLeftDoubleClick());
	}

	public void onRightDown(Position position) {
		this.position = position;
		setGesture(currentGesture.onRightDown());
	}

	public void onRightUp(Position position) {
		this.position = position;
		setGesture(currentGesture.onRightUp());
	}

	public void onRightDoubleClick(Position position) {
		this.position = position;
		setGesture(currentGesture.onRightDoubleClick());
	}

	public void onTouchStart(ArrayList<Touch> touches) {
		this.position = new Position(touches.get(0));
		setGesture(currentGesture.onTouchStart(touches));
	}

	public void onTouchMove(ArrayList<Touch> touches) {
		this.position = new Position(touches.get(0));
		setGesture(currentGesture.onTouchMove(touches));
	}

	public void onTouchEnd(ArrayList<Touch> touches) {
		setGesture(currentGesture.onTouchEnd(touches));
	}

	private void setGesture(Gesture g) {
		if (g != currentGesture) {

			if (currentReceiver != null) {
				if (!(g instanceof Gesture.NeedsFocus)) {
					resetReceiver();
				}
			}

			Control receiver = getReceiver(g);
			currentGesture = g;

			if (g != null) {
				if (!(g instanceof Identity)) {
					deliverGesture(receiver);
					Gesture follower = currentGesture.getFollower();

					if (follower != null)
						setGesture(follower);
					else if (currentGesture.isTerminus())
						reset();
				}
			}
		}
	}

	private void deliverGesture(Control receiver) {
		boolean handledByCaptureControl = false;

		if (receiver != null)
			if (receiver == currentReceiver)
				handledByCaptureControl = deliverGestureToCaptureControl();

		boolean isSpaceDown = NonMaps.get().getNonLinearWorld().isSpaceDown();

		if (!handledByCaptureControl && !isSpaceDown)
			bubbleGesture(receiver);
	}

	private boolean deliverGestureToCaptureControl() {
		if (currentReceiver.isVisible()) {
			Control r = currentGesture.applyTo(currentReceiver);
			if (r != null) {
				setReceiver(r);
				return true;
			}
		}
		return false;
	}

	private void bubbleGesture(Control receiver) {
		if (receiver != null) {
			setReceiver(receiver.handleGesture(currentGesture));
		}
	}

	private void reset() {
		resetReceiver();
		setGesture(new Identity());
	}

	public void removeReceiver() {
		resetReceiver();
	}

	private void resetReceiver() {
		if (currentReceiver != null)
			currentReceiver.gestureFocusLost();

		currentReceiver = null;
	}

	public void setReceiver(Control r) {
		if (r != null) {

			if (currentReceiver != r) {
				resetReceiver();
				currentReceiver = r;
				currentReceiver.gestureFocusCaptured();
			}
		}
	}

	private Control getReceiver(Gesture g) {
		if (NonMaps.get().getNonLinearWorld().isSpaceDown()) {
			Control world = NonMaps.theMaps.getNonLinearWorld();
			setReceiver(world);
			return world;
		}

		if (currentReceiver != null && g instanceof Gesture.NeedsFocus) {
			return currentReceiver;
		}

		if (g instanceof Gesture.NeedsFocus) {
			return null;
		}

		return NonMaps.theMaps.getNonLinearWorld();
	}

	void longPressTimerExpired(Gesture sender) {
		if (sender == currentGesture)
			setGesture(sender.onLongPress());
	}
}
