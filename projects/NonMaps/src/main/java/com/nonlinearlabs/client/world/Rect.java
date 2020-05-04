package com.nonlinearlabs.client.world;

import com.google.gwt.canvas.dom.client.Context2d;
import com.google.gwt.canvas.dom.client.Context2d.LineCap;
import com.google.gwt.canvas.dom.client.Context2d.LineJoin;
import com.nonlinearlabs.client.Millimeter;

public class Rect extends Object {
	private final Position pos = new Position();
	private final Dimension dim = new Dimension();

	public Rect() {
	}

	public Rect(Position position, Dimension dimension) {
		pos.set(position);
		dim.set(dimension);
	}

	public Rect(double x, double y, double w, double h) {
		this(new Position(x, y), new Dimension(w, h));
	}

	public Rect(Position a, Position b) {
		pos.setX(Math.min(a.getX(), b.getX()));
		pos.setY(Math.min(a.getY(), b.getY()));
		dim.setWidth(Math.abs(a.getX() - b.getX()));
		dim.setHeight(Math.abs(a.getY() - b.getY()));
	}

	public Rect getReducedBy(double d) {
		return new Rect(getLeft() + d / 2, getTop() + d / 2, getWidth() - d, getHeight() - d);
	}

	public double getLeft() {
		return pos.getX();
	}

	public double getTop() {
		return pos.getY();
	}

	public double getWidth() {
		return dim.getWidth();
	}

	public double getHeight() {
		return dim.getHeight();
	}

	private void setSize(double width, double height) {
		setWidth(width);
		setHeight(height);
	}

	private void setLocation(double left, double top) {
		setLeft(left);
		setTop(top);
	}

	public void setLeft(double left) {
		this.pos.setX(left);
	}

	public void setTop(double top) {
		this.pos.setY(top);
	}

	public boolean setWidth(double width) {
		return this.dim.setWidth(width);
	}

	public boolean setHeight(double height) {
		return this.dim.setHeight(height);
	}

	public void moveBy(double d, double e) {
		pos.moveBy(d, e);
	}

	public double getRight() {
		return getLeft() + getWidth();
	}

	public double getBottom() {
		return getTop() + getHeight();
	}

	private boolean intersects(double x, double y, double w, double h) {
		if (x > getRight() || getLeft() > (x + w))
			return false;

		if (y > getBottom() || getTop() > (y + h))
			return false;

		return true;
	}

	public Position getCenterPoint() {
		return new Position(getLeft() + getWidth() / 2, getTop() + getHeight() / 2);
	}

	public void moveTo(double x, double y) {
		setLocation(x, y);
	}

	public Position getPosition() {
		return pos;
	}

	public Dimension getDimension() {
		return dim;
	}

	@Override
	public String toString() {
		return "pos=" + pos + "; dim=" + dim;
	}

	public boolean contains(Position pos) {
		return getLeft() <= pos.getX() && getRight() >= pos.getX() && getTop() <= pos.getY()
				&& getBottom() >= pos.getY();
	}

	public void setDimension(Dimension dim) {
		this.dim.set(dim);
	}

	public boolean intersects(Rect r) {
		return intersects(r.getLeft(), r.getTop(), r.getWidth(), r.getHeight());
	}

	public boolean intersectsVertically(Rect r) {
		if (r.getTop() > getBottom() || getTop() > (r.getBottom()))
			return false;

		return true;
	}

	public Rect copy() {
		return new Rect(pos.copy(), dim.copy());
	}

	public void fill(Context2d ctx, RGB color) {
		ctx.setFillStyle(color.toString());
		ctx.fillRect(pos.getX(), pos.getY(), dim.getWidth(), dim.getHeight());
	}

	public void stroke(Context2d ctx, double strokeWidth, RGB color) {
		ctx.setLineWidth(strokeWidth);
		ctx.setStrokeStyle(color.toString());
		ctx.strokeRect(pos.getX(), pos.getY(), dim.getWidth(), dim.getHeight());
	}

	public void fillAndStroke(Context2d ctx, RGB fillColor, double strokeWidth, RGB strokeColor) {
		fill(ctx, fillColor);
		stroke(ctx, strokeWidth, strokeColor);
	}

	public void drawDebug(Context2d ctx) {
		// Draw Rect
		drawRoundedRect(ctx, 0, 0, 1, null, RGB.red());

		// Draw Center Point
		double centerSize = Millimeter.toPixels(2);
		ctx.beginPath();
		ctx.arc(getCenterPoint().getX(), getCenterPoint().getY(), centerSize, 0, 2 * Math.PI);
		ctx.closePath();
		ctx.stroke();

		// Draw Lines Through Center and extend indefinitely
		ctx.beginPath();
		ctx.moveTo(getCenterPoint().getX(), Integer.MIN_VALUE);
		ctx.lineTo(getCenterPoint().getX(), Integer.MAX_VALUE);
		ctx.moveTo(Integer.MIN_VALUE, getCenterPoint().getY());
		ctx.lineTo(Integer.MAX_VALUE, getCenterPoint().getY());
		ctx.closePath();
		ctx.stroke();
	}

	public void drawRoundedArea(Context2d ctx, double corner, double width, RGB fill, RGB stroke) {
		ctx.beginPath();
		ctx.moveTo(getLeft() + corner, getTop());
		ctx.lineTo(getRight() - corner, getTop());
		ctx.arcTo(getRight(), getTop(), getRight(), getTop() + corner, corner);
		ctx.lineTo(getRight(), getBottom() - corner);
		ctx.arcTo(getRight(), getBottom(), getRight() - corner, getBottom(), corner);
		ctx.lineTo(getLeft() + corner, getBottom());
		ctx.arcTo(getLeft(), getBottom(), getLeft(), getBottom() - corner, corner);
		ctx.lineTo(getLeft(), getTop() + corner);
		ctx.arcTo(getLeft(), getTop(), getLeft() + corner, getTop(), corner);
		ctx.closePath();

		if (fill != null) {
			ctx.setFillStyle(fill.toString());
			ctx.fill();
		}

		if (stroke != null) {
			ctx.setLineWidth(Math.ceil(width));
			ctx.setStrokeStyle(stroke.toString());
			ctx.stroke();
		}
	}

	public void reduceWidthBy(double pix) {
		if (getLeft() < getRight()) {
			getPosition().setX(getPosition().getX() + pix / 2);
			getDimension().setWidth(getDimension().getWidth() - pix);
		} else if (getLeft() > getRight()) {
			getPosition().setX(getPosition().getX() - pix / 2);
			getDimension().setWidth(getDimension().getWidth() + pix);
		}
	}

	public void reduceHeightBy(double pix) {
		if (getTop() < getBottom()) {
			getPosition().setY(getPosition().getY() + pix / 2);
			getDimension().setHeight(getDimension().getHeight() - pix);
		}
		if (getTop() > getBottom()) {
			getPosition().setY(getPosition().getY() - pix / 2);
			getDimension().setHeight(getDimension().getHeight() + pix);
		}
	}

	public void normalize() {
		double l = Math.min(getLeft(), getRight());
		double r = Math.max(getLeft(), getRight());
		double t = Math.min(getTop(), getBottom());
		double b = Math.max(getTop(), getBottom());
		setLocation(l, t);
		setSize(r - l, b - t);
	}

	public void centerIn(Rect pixRect) {
		moveBy((pixRect.getWidth() - getWidth()) / 2, (pixRect.getHeight() - getHeight()) / 2);
	}

	public Rect getIntersection(Rect pixRect) {
		double l = Math.max(getLeft(), pixRect.getLeft());
		double t = Math.max(getTop(), pixRect.getTop());
		double r = Math.min(getRight(), pixRect.getRight());
		double b = Math.min(getBottom(), pixRect.getBottom());
		double w = r - l;
		double h = b - t;
		return new Rect(l, t, w >= 0 ? w : 0, h >= 0 ? h : 0);
	}

	public void setRight(double right) {
		setWidth(right - getLeft());
	}

	public void applyPadding(double leftPadding, double topPadding, double rightPadding, double bottomPadding) {
		getPosition().moveBy(leftPadding, topPadding);
		getDimension().add(-(rightPadding + leftPadding), -(topPadding + bottomPadding));
	}

	public boolean isEmpty(double tolerance) {
		return getWidth() <= tolerance || getHeight() <= tolerance;
	}

	public void set(double x, double y, double w, double h) {
		getPosition().set(x, y);
		getDimension().set(w, h);
	}

	public void set(Rect rect) {
		set(rect.getLeft(), rect.getTop(), rect.getWidth(), rect.getHeight());
	}

	public void setBottom(double bottom) {
		setHeight(bottom - getTop());
	}

	static public final int ROUNDING_LEFT_TOP = 1;
	static public final int ROUNDING_RIGHT_TOP = 2;
	static public final int ROUNDING_LEFT_BOTTOM = 4;
	static public final int ROUNDING_RIGHT_BOTTOM = 8;
	static public final int ROUNDING_TOP = ROUNDING_LEFT_TOP | ROUNDING_RIGHT_TOP;
	static public final int ROUNDING_BOTTOM = ROUNDING_LEFT_BOTTOM | ROUNDING_RIGHT_BOTTOM;
	static public final int ROUNDING_LEFT = ROUNDING_LEFT_TOP | ROUNDING_LEFT_BOTTOM;
	static public final int ROUNDING_RIGHT = ROUNDING_RIGHT_TOP | ROUNDING_RIGHT_BOTTOM;
	static public final int ROUNDING_ALL = ROUNDING_TOP | ROUNDING_BOTTOM;
	static public final int ROUNDING_NONE = 0;

	public void drawRoundedRect(Context2d ctx, int roundings, double corner, double width, RGB fill, RGB stroke) {
		createRoundedRectPath(ctx, roundings, corner);

		if (fill != null) {
			ctx.setFillStyle(fill.toString());
			ctx.fill();
		}

		if (stroke != null) {
			ctx.setLineWidth(Math.ceil(width));
			ctx.setStrokeStyle(stroke.toString());
			ctx.stroke();
		}
	}

	public void createRoundedRectPath(Context2d ctx, int roundings, double corner) {
		ctx.beginPath();

		if ((roundings & ROUNDING_LEFT_TOP) != 0) {
			ctx.moveTo(getLeft() + corner, getTop());
		} else {
			ctx.moveTo(getLeft(), getTop());
		}

		if ((roundings & ROUNDING_RIGHT_TOP) != 0) {
			ctx.lineTo(getRight() - corner, getTop());
			ctx.arcTo(getRight(), getTop(), getRight(), getTop() + corner, corner);

		} else {
			ctx.lineTo(getRight(), getTop());
		}

		if ((roundings & ROUNDING_RIGHT_BOTTOM) != 0) {
			ctx.lineTo(getRight(), getBottom() - corner);
			ctx.arcTo(getRight(), getBottom(), getRight() - corner, getBottom(), corner);

		} else {
			ctx.lineTo(getRight(), getBottom());
		}

		if ((roundings & ROUNDING_LEFT_BOTTOM) != 0) {
			ctx.lineTo(getLeft() + corner, getBottom());
			ctx.arcTo(getLeft(), getBottom(), getLeft(), getBottom() - corner, corner);

		} else {
			ctx.lineTo(getLeft(), getBottom());
		}

		if ((roundings & ROUNDING_LEFT_TOP) != 0) {
			ctx.lineTo(getLeft(), getTop() + corner);
			ctx.arcTo(getLeft(), getTop(), getLeft() + corner, getTop(), corner);

		} else {
			ctx.lineTo(getLeft(), getTop());
		}

		ctx.closePath();
	}

	public boolean contains(Rect pixRect) {
		return contains(pixRect.getLeftTop()) && contains(pixRect.getRightBottom());
	}

	public Position getRightBottom() {
		return new Position(getLeft() + getWidth(), getTop() + getHeight());
	}

	public Position getLeftTop() {
		return pos.copy();
	}

	public void drawValueEditSliderBackgound(Context2d ctx, boolean withArrows, RGB arrowColor) {
		drawRoundedArea(ctx, getHeight() / 2, 1, new Gray(68), new Gray(86));

		if (withArrows) {
			ctx.setStrokeStyle(arrowColor.toString());
			ctx.setLineWidth(2);
			ctx.setLineCap(LineCap.SQUARE);
			ctx.setLineJoin(LineJoin.MITER);
			double arrowWidth = getHeight() / 4;
			double arrowHeight = getHeight() / 2;
			double arrowMargin = 2 * arrowWidth;

			ctx.beginPath();
			ctx.moveTo(getLeft() + arrowMargin + arrowWidth, getCenterPoint().getY() - arrowHeight / 2);
			ctx.lineTo(getLeft() + arrowMargin, getCenterPoint().getY());
			ctx.lineTo(getLeft() + arrowMargin + arrowWidth, getCenterPoint().getY() + arrowHeight / 2);

			ctx.moveTo(getRight() - arrowMargin - arrowWidth, getCenterPoint().getY() - arrowHeight / 2);
			ctx.lineTo(getRight() - arrowMargin, getCenterPoint().getY());
			ctx.lineTo(getRight() - arrowMargin - arrowWidth, getCenterPoint().getY() + arrowHeight / 2);

			ctx.stroke();
		}

	}

	public void moveBy(Position position) {
		moveBy(position.getX(), position.getY());
	}

	public double getDistanceTo(Position pos) {
		if (contains(pos))
			return 0;

		return getCenterPoint().getDistanceTo(pos);
	}

	public Position getRightTop() {
		return new Position(getRight(), getTop());
	}

	public Position getLeftBottom() {
		return new Position(getLeft(), getBottom());
	}

	public Rect getMovedBy(Dimension offset) {
		Rect a = copy();
		a.moveBy(offset.getWidth(), offset.getHeight());
		return a;
	}

	public void round() {
		pos.round();
	}
}
