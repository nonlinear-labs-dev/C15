package com.nonlinearlabs.client.world.overlay.belt.presets;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.contextStates.ClipContext;
import com.nonlinearlabs.client.presenters.PresetManagerPresenterProvider;
import com.nonlinearlabs.client.useCases.BankUseCases;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Gray;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.RGBA;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.overlay.DragProxy;
import com.nonlinearlabs.client.world.overlay.Overlay;
import com.nonlinearlabs.client.world.overlay.OverlayLayout;

class BankHeader extends OverlayLayout {

	private class OverlayBankContextMenu extends BankContextMenu {
		private OverlayBankContextMenu(OverlayLayout parent, String bank) {
			super(parent, bank);
		}

		@Override
		protected boolean hasCollapse() {
			return false;
		}

		@Override
		protected boolean hasPaste() {
			return false;
		}

		@Override
		protected boolean hasBankCreationRights() {
			return false;
		}

	}

	private PreviousBank prev;
	private BankTitle title;
	private NextBank next;
	private boolean isDropTarget = false;

	BankHeader(BankControl parent) {
		super(parent);
		addChild(prev = new PreviousBank(this));
		addChild(next = new NextBank(this));
		addChild(title = new BankTitle(this));
	}

	@Override
	public BankControl getParent() {
		return (BankControl) super.getParent();
	}

	@Override
	public void draw(Context2d ctx, Context2d overlay, int invalidationMask) {
		Rect r = new Rect(prev.getPixRect().getLeft(), prev.getPixRect().getTop(),
				next.getPixRect().getRight() - prev.getPixRect().getLeft(),
				next.getPixRect().getBottom() - prev.getPixRect().getTop());
		r.reduceHeightBy(-2);
		r.drawRoundedArea(ctx, 0, 1, new Gray(106), new Gray(106));
		super.draw(ctx, overlay, invalidationMask);

		if (isDropTarget)
			drawTeeth(ctx);

	}

	protected void drawTeeth(Context2d ctx) {
		try (ClipContext clipper = new ClipContext(ctx, this)) {
			drawClippedTeeth(ctx);
		}
	}

	protected void drawClippedTeeth(Context2d ctx) {
		ctx.setFillStyle(new RGBA(255, 0, 0, 0.5).toString());
		Rect r = getPixRect();

		double dim = r.getHeight() / 2;
		for (double x = r.getLeft(); x < r.getRight(); x += dim)
			drawTooth(ctx, x, r.getCenterPoint().getY() - dim / 2, dim);
	}

	private void drawTooth(Context2d ctx, double x, double y, double dim) {
		ctx.beginPath();
		ctx.moveTo(x, y);
		ctx.lineTo(x + dim, y);
		ctx.lineTo(x + dim / 2, y + dim);
		ctx.closePath();
		ctx.fill();
	}

	@Override
	public void doLayout(double x, double y, double w, double h) {
		super.doLayout(x, y, w, h);

		prev.doLayout(0, 0, h, h);
		next.doLayout(w - h, 0, h, h);
		double titleLeft = prev.getRelativePosition().getRight() + 0.5;
		double titleWidth = next.getRelativePosition().getLeft() - titleLeft;
		title.doLayout(titleLeft, 0, titleWidth, h);
	}

	@Override
	public Control drag(Position pos, DragProxy dragProxy) {
		if (!getPixRect().contains(pos))
			return null;

		// todo
		// if (dragProxy.getOrigin() instanceof IPreset || dragProxy.getOrigin()
		// instanceof EditBufferDraggingButton
		// || dragProxy.getOrigin() instanceof IBank) {
		// setIsDropTarget(true);
		// return this;
		// }
		return super.drag(pos, dragProxy);
	}

	@Override
	public void dragLeave() {
		setIsDropTarget(false);
		super.dragLeave();
	}

	private void setIsDropTarget(boolean isDropTarget) {
		if (this.isDropTarget != isDropTarget) {
			this.isDropTarget = isDropTarget;
			invalidate(INVALIDATION_FLAG_UI_CHANGED);
		}
	}

	@Override
	public Control drop(Position pos, DragProxy dragProxy) {
		BankUseCases.get().dropOnBank(getParent().getBankPresenter().uuid);
		return this;
	}

	@Override
	public Control onContextMenu(Position pos) {
		if (PresetManagerPresenterProvider.get().getPresenter().inStoreSelectMode)
			return null;

		Overlay o = NonMaps.theMaps.getNonLinearWorld().getViewport().getOverlay();
		return o.setContextMenu(pos, new OverlayBankContextMenu(o, getParent().getBankPresenter().uuid));
	}

	public void setFontHeightInMM(int mm) {
		((BankTitle) title).setFontHeightInMM(mm);
	}

	public void doLayout(double x, double y, double w) {
		doLayout(x, y, w, prev.getSelectedImage().getImgHeight());

	}
}
