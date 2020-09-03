package com.nonlinearlabs.client.world.maps;

import com.google.gwt.canvas.dom.client.Context2d;
import com.google.gwt.core.client.Scheduler;
import com.nonlinearlabs.client.Animator;
import com.nonlinearlabs.client.Animator.DoubleClientData.Client;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;

public class ContextMenu extends LayoutResizingVertical implements IContextMenu {

	private double opacity = 1.0;
	private Animator fadeOut;

	public ContextMenu(MapsLayout parent) {
		super(parent);
	}

	@Override
	public Control mouseDrag(Position oldPoint, Position newPoint, boolean fine) {
		return this;
	}

	@Override
	public Control mouseUp(Position eventPoint) {
		scheduleFadeOut(2000);
		return this;
	}

	private void scheduleFadeOut(int delay) {
		Scheduler.get().scheduleFixedDelay(new Scheduler.RepeatingCommand() {

			@Override
			public boolean execute() {
				if (fadeOut == null)
					fadeOut();

				return false;
			}
		}, delay);
	}

	@Override
	public void draw(Context2d ctx, Context2d overlay, int invalidationMask) {
		ctx.setGlobalAlpha(opacity);
		super.draw(ctx, overlay, invalidationMask);
		ctx.setGlobalAlpha(1.0);
	}

	public void hide() {
		getParent().removeChild(this);
		getParent().requestLayout();
		fadeOut.cancel();
	}

	void fadeOut() {
		fadeOut(1500);
	}

	public void fadeOut(int timeMS) {
		fadeOut = new Animator(timeMS);
		fadeOut.addSubAnimation(opacity, 0, new Client() {

			@Override
			public void animate(double v) {
				opacity = v;
				invalidate(INVALIDATION_FLAG_UI_CHANGED);

				if (v == 0.0)
					hide();
			}
		});
	}

	protected NonDimension layoutChildren(double levelOfDetail) {
		NonDimension ret = super.layoutChildren(levelOfDetail);

		for (MapsControl c : getChildren()) {
			c.getNonPosition().getPosition().setX(0);
			c.getNonPosition().getDimension().setWidth(ret.getWidth());
		}

		return ret;
	}
}
