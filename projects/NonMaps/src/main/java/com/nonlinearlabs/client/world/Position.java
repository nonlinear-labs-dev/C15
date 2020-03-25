package com.nonlinearlabs.client.world;

import com.google.gwt.dom.client.NativeEvent;
import com.google.gwt.event.dom.client.MouseEvent;
import com.google.gwt.event.shared.EventHandler;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.world.pointer.Touch;

public class Position {

	private double x = 0;
	private double y = 0;

	public Position() {
	}

	public Position(double x, double y) {
		this.x = x;
		this.y = y;
	}

	public <H extends EventHandler> Position(MouseEvent<H> event) {
		final double r = NonMaps.devicePixelRatio;
		this.x = event.getClientX() * r;
		this.y = event.getClientY() * r;
	}

	public Position(Touch touch) {
		this.x = touch.pos.getX();
		this.y = touch.pos.getY();
	}

	public Position(NativeEvent e) {
		this(e.getClientX() * NonMaps.devicePixelRatio, e.getClientY() * NonMaps.devicePixelRatio);
	}

	public void moveBy(double x, double y) {
		this.x += x;
		this.y += y;
	}

	public void moveBy(Position other) {
		this.x += other.getX();
		this.y += other.getY();
	}

	public double getX() {
		return x;
	}

	public double getY() {
		return y;
	}

	public Position copy() {
		return new Position(x, y);
	}

	public void setX(double x) {
		this.x = x;
	}

	public void setY(double y) {
		this.y = y;
	}

	public void set(Position position) {
		this.x = position.getX();
		this.y = position.getY();
	}

	@Override
	public String toString() {
		return "x=" + x + ", y=" + y;
	}

	public Position getNegated() {
		return new Position(-getX(), -getY());
	}

	void round() {
		x = Math.round(x);
		y = Math.round(y);
	}

	public double getDistanceTo(Position b) {
		double diffX = getX() - b.getX();
		double diffY = getY() - b.getY();
		return Math.sqrt(diffX * diffX + diffY * diffY);
	}

	public Dimension getVector(Position b) {
		double diffX = getX() - b.getX();
		double diffY = getY() - b.getY();
		return new Dimension(diffX, diffY);
	}

	public Position getMiddle(Position b) {
		return new Position((getX() + b.getX()) / 2, (getY() + b.getY()) / 2);
	}

	void set(double x, double y) {
		this.x = x;
		this.y = y;
	}

}
