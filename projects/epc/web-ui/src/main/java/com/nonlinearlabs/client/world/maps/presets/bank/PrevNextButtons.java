package com.nonlinearlabs.client.world.maps.presets.bank;

import com.google.gwt.canvas.dom.client.Context2d;
import com.google.gwt.canvas.dom.client.Context2d.LineCap;
import com.google.gwt.canvas.dom.client.Context2d.LineJoin;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Gray;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.maps.LayoutResizingHorizontal;
import com.nonlinearlabs.client.world.maps.NonPosition;
import com.nonlinearlabs.client.world.maps.parameters.ZoomReactingControl;

class PrevNextButtons extends LayoutResizingHorizontal {

	private abstract class Arrow extends ZoomReactingControl {
		private boolean mouseDown = false;

		private Arrow(PrevNextButtons parent) {
			super(parent);
		}

		@Override
		public PrevNextButtons getParent() {
			return (PrevNextButtons) super.getParent();
		}

		@Override
		protected double getBasicWidth() {
			return 60;
		}

		@Override
		protected double getBasicHeight() {
			return 30;
		}

		@Override
		protected double getMinHeight() {
			return 0;
		}

		@Override
		public Control mouseDown(Position eventPoint) {
			if (isActive())
				return this;

			return null;
		}

		@Override
		public Control mouseUp(Position eventPoint) {
			return this;
		}

		protected abstract boolean isActive();

		@Override
		public void onMouseCaptured() {
			mouseDown = true;
			invalidate(INVALIDATION_FLAG_UI_CHANGED);
		}

		@Override
		public void onMouseLost() {
			mouseDown = false;
			invalidate(INVALIDATION_FLAG_UI_CHANGED);
		}

		protected RGB getArrowColor() {
			if (!isActive())
				return new Gray(0);
			if (mouseDown)
				return new Gray(230);
			return new Gray(204);
		}

		protected RGB getBackgroundColor() {
			if (!isActive())
				return new Gray(51);
			if (mouseDown)
				return new Gray(128);
			return new Gray(77);
		}

		@Override
		public void draw(Context2d ctx, Context2d overlay, int invalidationMask) {
			super.draw(ctx, overlay, invalidationMask);

			RGB bg = getBackgroundColor();
			RGB arrow = getArrowColor();

			Rect r = getPixRect();
			r.fill(ctx, bg);

			ctx.setStrokeStyle(arrow.toString());
			ctx.setLineWidth(toXPixels(2));
			ctx.setLineCap(LineCap.SQUARE);
			ctx.setLineJoin(LineJoin.MITER);
		}
	}

	private final class LeftArrow extends Arrow {
		private LeftArrow(PrevNextButtons parent) {
			super(parent);
		}

		@Override
		public Control click(Position eventPoint) {
			getParent().getParent().getPresetList().selectPrev();
			return this;
		}

		@Override
		protected boolean isActive() {
			return getParent().getParent().getPresetList().canPrev();
		}

		@Override
		public void draw(Context2d ctx, Context2d overlay, int invalidationMask) {
			super.draw(ctx, overlay, invalidationMask);

			Rect r = getPixRect();
			Position p = r.getCenterPoint();

			double arrowWidth = r.getHeight() / 8;
			double arrowHeight = r.getHeight() / 2;

			ctx.beginPath();
			ctx.moveTo(p.getX() + arrowWidth, p.getY() - arrowHeight / 2);
			ctx.lineTo(p.getX() - arrowWidth, p.getY());
			ctx.lineTo(p.getX() + arrowWidth, p.getY() + arrowHeight / 2);
			ctx.stroke();
		}
	}

	private final class RightArrow extends Arrow {
		private RightArrow(PrevNextButtons parent) {
			super(parent);
		}

		@Override
		public Control click(Position eventPoint) {
			getParent().getParent().getPresetList().selectNext();
			return this;
		}

		@Override
		protected boolean isActive() {
			return getParent().getParent().getPresetList().canNext();
		}

		@Override
		public void draw(Context2d ctx, Context2d overlay, int invalidationMask) {
			super.draw(ctx, overlay, invalidationMask);

			Rect r = getPixRect();
			Position p = r.getCenterPoint();

			double arrowWidth = r.getHeight() / 8;
			double arrowHeight = r.getHeight() / 2;

			ctx.beginPath();
			ctx.moveTo(p.getX() - arrowWidth, p.getY() - arrowHeight / 2);
			ctx.lineTo(p.getX() + arrowWidth, p.getY());
			ctx.lineTo(p.getX() - arrowWidth, p.getY() + arrowHeight / 2);
			ctx.stroke();
		}
	}

	private Arrow prev;
	private Arrow next;

	PrevNextButtons(Bank parent) {
		super(parent);
		prev = addChild(new LeftArrow(this));
		next = addChild(new RightArrow(this));
	}

	@Override
	public Bank getParent() {
		return (Bank) super.getParent();
	}

	@Override
	public void doFirstLayoutPass(double levelOfDetail) {
		super.doFirstLayoutPass(levelOfDetail);

		if (!getParent().isCollapsed())
			getNonPosition().getDimension().setHeight(0);
	}

	@Override
	public void doSecondLayoutPass(double parentsWidthFromFirstPass, double parentsHeightFromFirstPass) {
		prev.setNonSize(parentsWidthFromFirstPass / 2, getNonPosition().getHeight());
		next.setNonSize(parentsWidthFromFirstPass / 2, getNonPosition().getHeight());
		next.getNonPosition()
				.setPosition(new NonPosition(prev.getNonPosition().getRight(), prev.getNonPosition().getTop()));
		setNonSize(parentsWidthFromFirstPass, getNonPosition().getHeight());
	}

	@Override
	public void draw(Context2d ctx, Context2d overlay, int invalidationMask) {
		Rect pix = getPixRect();

		RGB colorContour = new RGB(0, 0, 0);
		RGB colorFill = new RGB(25, 25, 25);

		double cp = getConturPixels();

		pix.fillAndStroke(ctx, colorFill, cp, colorContour);

		super.draw(ctx, overlay, invalidationMask);

		Rect highlighRect = getPixRect().getReducedBy(cp / 2);
		highlighRect.stroke(ctx, cp, getParent().getColorBankInnerBorder());

		Rect blackRect = getPixRect().getReducedBy(5 * cp / 2);
		blackRect.stroke(ctx, cp, colorContour);

		ctx.beginPath();
		ctx.setStrokeStyle(getParent().getColorBankInnerBorder().toString());
		ctx.moveTo(pix.getCenterPoint().getX(), pix.getTop());
		ctx.lineTo(pix.getCenterPoint().getX(), pix.getBottom());
		ctx.stroke();

		ctx.setStrokeStyle(colorContour.toString());
		ctx.beginPath();
		ctx.moveTo(pix.getCenterPoint().getX() + cp, pix.getTop() + cp);
		ctx.lineTo(pix.getCenterPoint().getX() + cp, pix.getBottom() - cp);
		ctx.stroke();

		ctx.beginPath();
		ctx.moveTo(pix.getCenterPoint().getX() - cp, pix.getTop() + cp);
		ctx.lineTo(pix.getCenterPoint().getX() - cp, pix.getBottom() - cp);
		ctx.stroke();

	}
}
