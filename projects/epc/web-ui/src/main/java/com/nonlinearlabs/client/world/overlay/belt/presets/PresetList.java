package com.nonlinearlabs.client.world.overlay.belt.presets;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.Animator;
import com.nonlinearlabs.client.Animator.DoubleClientData.Client;
import com.nonlinearlabs.client.Millimeter;
import com.nonlinearlabs.client.contextStates.ClipContext;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.VoiceGroup;
import com.nonlinearlabs.client.presenters.BankPresenter;
import com.nonlinearlabs.client.presenters.BankPresenterProviders;
import com.nonlinearlabs.client.presenters.PresetManagerPresenterProvider;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.overlay.DragProxy;
import com.nonlinearlabs.client.world.overlay.OverlayControl;
import com.nonlinearlabs.client.world.overlay.OverlayLayout;
import com.nonlinearlabs.client.world.pointer.PointerState;

public class PresetList extends OverlayLayout {

	public enum ScrollRequest {
		Jump, Smooth, None
	}

	private boolean isDropTarget = false;
	private double scrollPosition = 0;
	private Animator animation;
	private ScrollRequest scrollRequest = ScrollRequest.None;

	private String selectedBank = "";
	private String selectedPreset = "";
	private VoiceGroup selectedPart = VoiceGroup.Global;

	private BankPresenter bankPresenter = new BankPresenter();

	protected PresetList(BankControl parent) {
		super(parent);

		PresetManagerPresenterProvider.get().register(p -> {
			select(p.selectedBank, p.selectedPreset, p.selectedPart);
			return true;
		});
	}

	private void select(String selectedBank, String selectedPreset, VoiceGroup selectedPart) {
		if (selectedBank != this.selectedBank) {
			this.selectedBank = selectedBank;

			BankPresenterProviders.get().register(this.selectedBank, b -> {
				if (b.uuid != this.selectedBank)
					return false;

				bankPresenter = b;
				select(selectedPreset, selectedPart);
				scheduleAutoScroll(ScrollRequest.Jump);
				return true;
			});
		}

		syncPresets();
		select(selectedPreset, selectedPart);
	}

	private void syncPresets() {
		var numDesiredPresets = bankPresenter.presets.size();
		var numExistingPresets = getChildren().size();

		while (numDesiredPresets > numExistingPresets) {
			addChild(new BeltPreset(this));
			numExistingPresets++;
		}

		while (numDesiredPresets < numExistingPresets) {
			removeChild(getChildren().get(0));
			numExistingPresets--;
		}

		for (int i = 0; i < numDesiredPresets; i++)
			((BeltPreset) getChildren().get(i)).setOrigin(bankPresenter.presets.get(i));
	}

	private void select(String selectedPreset, VoiceGroup selectedPart) {
		if (this.selectedPreset != selectedPreset || this.selectedPart != selectedPart) {
			this.selectedPreset = selectedPreset;
			this.selectedPart = selectedPart;
			scheduleAutoScroll(ScrollRequest.Smooth);
		}
	}

	@Override
	public BankControl getParent() {
		return (BankControl) super.getParent();
	}

	@Override
	public void doLayout(double x, double y, double w, double h) {
		super.doLayout(x, y, w, h);
		layoutChildren();
	}

	private void layoutChildren() {
		double childHeight = getChildHeight();
		double maxScrollPos = childHeight;
		double minScrollPos = -getChildren().size() * childHeight + getRelativePosition().getHeight() - childHeight;
		scrollPosition = Math.min(maxScrollPos, scrollPosition);
		scrollPosition = Math.max(minScrollPos, scrollPosition);

		double childY = scrollPosition;
		boolean canAutoscroll = true;

		for (OverlayControl c : getChildren()) {
			if (canAutoscroll)
				if (PointerState.get().isCurrentReceiver(c))
					canAutoscroll = false;

			c.doLayout(0, childY, getRelativePosition().getWidth(), childHeight);
			childY += childHeight;
		}

		if (scrollRequest != ScrollRequest.None && canAutoscroll) {
			autoScrollToSelected();
			scrollRequest = ScrollRequest.None;
		}
	}

	private double getChildHeight() {
		return getRelativePosition().getHeight() / 3;
	}

	private void drawDropIndicator(Context2d ctx, Rect rect, double yOffset, double heightFactor) {
		ctx.fillRect(rect.getLeft(), rect.getTop() + rect.getHeight() * yOffset, rect.getWidth(),
				rect.getHeight() * heightFactor);
	}

	@Override
	public void draw(Context2d ctx, Context2d overlay, int invalidationMask) {
		try (ClipContext clip = new ClipContext(ctx, this)) {
			super.draw(ctx, overlay, invalidationMask);

			ctx.setFillStyle("rgba(255, 0, 0, 0.5)");

			for (OverlayControl c : getChildren()) {
				if (c instanceof BeltPreset) {
					BeltPreset e = (BeltPreset) c;

					switch (e.getDropPosition()) {
						case TOP:
							drawDropIndicator(ctx, e.getPixRect(), -0.1, 0.2);
							break;

						case MIDDLE:
							drawDropIndicator(ctx, e.getPixRect(), 0.1, 0.8);
							break;

						case BOTTOM:
							drawDropIndicator(ctx, e.getPixRect(), 0.9, 0.2);
							break;

						default:
							break;
					}
				}
			}
		}
	}

	@Override
	public Control drag(Position pos, DragProxy dragProxy) {
		if (!getPixRect().contains(pos))
			return null;

		// TODO
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
		// TODO

		// if (dragProxy.getOrigin() instanceof IPreset) {
		// } else if (dragProxy.getOrigin() instanceof EditBufferDraggingButton)
		// getNonMaps().getServerProxy().dropEditBufferOnBank(bank);
		// else if (dragProxy.getOrigin() instanceof IBank) {
		// Bank draggedBank = (Bank) dragProxy.getOrigin();
		// if (!draggedBank.hasSlaves()) {
		// var bank = (IBank) dragProxy.getOrigin();
		// getNonMaps().getServerProxy().dropBankOnBank(bank.getUUID(), bank);
		// }
		// }

		// setIsDropTarget(false);
		return this;
	}

	public void drawTriangles(Context2d ctx, double top, double lineHeight) {
		Rect pix = getPixRect();
		ctx.setStrokeStyle(new RGB(205, 22, 22).toString());

		double space = Millimeter.toPixels(1.5);
		double width = Millimeter.toPixels(2);

		ctx.beginPath();
		for (double x = pix.getLeft(); x < pix.getRight(); x += space + width) {
			ctx.moveTo(x, top);
			ctx.lineTo(x + width, top);
		}

		ctx.setLineWidth(lineHeight);
		ctx.stroke();
	}

	public void autoScrollToSelected() {
		BeltPreset p = findSelectedPreset();
		if (p != null)
			scrollTo(p);
	}

	private BeltPreset findSelectedPreset() {
		for (var a : getChildren()) {
			BeltPreset p = (BeltPreset) a;
			if (p.getUuid() == selectedPreset)
				return p;
		}
		return null;
	}

	private void scrollTo(BeltPreset p) {
		if (scrollRequest == ScrollRequest.None)
			return;

		double target = getChildren().get(0).getRelativePosition().getTop() - p.getRelativePosition().getTop()
				+ getChildHeight();

		if (target != scrollPosition) {

			if (ButtonRepeat.get().isRunning() || scrollRequest == ScrollRequest.Jump) {
				doJumpScroll(target);
			} else {
				startScrollAnimation(target);
			}
		}
	}

	private void doJumpScroll(double target) {
		if (scrollPosition != target) {
			cancelScrollAnimation();
			scrollPosition = target;
			requestLayout();
		}
	}

	public void scheduleAutoScroll(ScrollRequest r) {
		if (scrollRequest == ScrollRequest.None) {
			scrollRequest = r;

			if (scrollRequest != ScrollRequest.None)
				requestLayout();
		}
	}

	private void startScrollAnimation(final double target) {
		if (scrollPosition != target) {
			animation = new Animator(500);

			animation.addSubAnimation(scrollPosition, target, new Client() {

				@Override
				public void animate(double v) {
					scrollPosition = v;

					if (v == target) {
						scrollRequest = ScrollRequest.None;
					}
					requestLayout();
				}
			});
		}
	}

	private void cancelScrollAnimation() {
		if (animation != null)
			animation.cancel();
		animation = null;
		scrollRequest = ScrollRequest.None;
	}

	@Override
	public boolean isVisible() {
		if (isIntendedVisible()) {
			if (getPixRect().getHeight() > 0 && getPixRect().getWidth() > 0) {
				return getParent().isRectVisible(getPixRect());
			}
		}
		return false;
	}

	@Override
	public boolean isRectVisible(Rect r) {
		return isVisible() && getPixRect().intersects(r);
	}

	public double getHorizontalCenterLinePosition() {
		return getRelativePosition().getCenterPoint().getY();
	}
}