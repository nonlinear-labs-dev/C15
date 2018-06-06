package com.nonlinearlabs.NonMaps.client.world.overlay.belt.presets;

import java.util.Iterator;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.NonMaps.client.Animator;
import com.nonlinearlabs.NonMaps.client.Animator.DoubleClientData.Client;
import com.nonlinearlabs.NonMaps.client.Millimeter;
import com.nonlinearlabs.NonMaps.client.contextStates.ClipContext;
import com.nonlinearlabs.NonMaps.client.world.Control;
import com.nonlinearlabs.NonMaps.client.world.IBank;
import com.nonlinearlabs.NonMaps.client.world.IPreset;
import com.nonlinearlabs.NonMaps.client.world.Position;
import com.nonlinearlabs.NonMaps.client.world.RGB;
import com.nonlinearlabs.NonMaps.client.world.Rect;
import com.nonlinearlabs.NonMaps.client.world.maps.MapsControl;
import com.nonlinearlabs.NonMaps.client.world.maps.presets.bank.Bank;
import com.nonlinearlabs.NonMaps.client.world.maps.presets.bank.preset.Preset;
import com.nonlinearlabs.NonMaps.client.world.overlay.DragProxy;
import com.nonlinearlabs.NonMaps.client.world.overlay.OverlayControl;
import com.nonlinearlabs.NonMaps.client.world.overlay.OverlayLayout;
import com.nonlinearlabs.NonMaps.client.world.overlay.belt.EditBufferDraggingButton;
import com.nonlinearlabs.NonMaps.client.world.pointer.PointerState;

public class PresetList extends OverlayLayout {

	public enum ScrollRequest {
		Jump, Smooth, None
	}

	private boolean isDropTarget = false;
	private double scrollPosition = 0;
	private Animator animation;
	private ScrollRequest scrollRequest = ScrollRequest.None;
	private BeltPreset currentScrollPreset;
	private Bank currentBank;

	protected PresetList(BankControl parent) {
		super(parent);
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
		ctx.fillRect(rect.getLeft(), rect.getTop() + rect.getHeight() * yOffset, rect.getWidth(), rect.getHeight() * heightFactor);
	}

	@Override
	public void draw(Context2d ctx, int invalidationMask) {
		try (ClipContext clip = new ClipContext(ctx, this)) {
			super.draw(ctx, invalidationMask);

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

		if (dragProxy.getOrigin() instanceof IPreset || dragProxy.getOrigin() instanceof EditBufferDraggingButton
				|| dragProxy.getOrigin() instanceof IBank) {
			setIsDropTarget(true);
			return this;
		}
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
		Bank b = getParent().getBankInCharge();

		if (dragProxy.getOrigin() instanceof IPreset)
			getNonMaps().getServerProxy().dropPresetOnBank((IPreset) dragProxy.getOrigin(), b);
		else if (dragProxy.getOrigin() instanceof EditBufferDraggingButton)
			getNonMaps().getServerProxy().dropEditBufferOnBank(b);
		else if (dragProxy.getOrigin() instanceof IBank) {
			Bank draggedBank = (Bank) dragProxy.getOrigin();
			if (!draggedBank.hasSlaves()) {
				getNonMaps().getServerProxy().dropBankOnBank((IBank) dragProxy.getOrigin(), b);
			}
		}

		setIsDropTarget(false);
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

	public void update() {
		Bank bank = getParent().getBankInCharge();
		boolean bankChanged = bank != currentBank;

		if (bankChanged)
			currentBank = bank;

		if (bank == null) {
			removeAllPresets();
		} else {
			Iterator<MapsControl> source = bank.getPresetList().getChildren().iterator();
			Iterator<OverlayControl> target = getChildren().iterator();

			updateExistingPresets(source, target);
			removeRemainders(target);
			addMissingPresets(source);
		}

		if (bankChanged) {
			scheduleAutoScroll(ScrollRequest.Jump);
		} else {
			scheduleAutoScroll(ScrollRequest.Smooth);
		}
	}

	public void autoScrollToSelected() {
		BeltPreset p = findSelectedPreset();
		if (p != null)
			scrollTo(p);
	}

	private void scrollTo(BeltPreset p) {
		if (currentScrollPreset == p || scrollRequest == ScrollRequest.None)
			return;

		currentScrollPreset = p;

		double target = getChildren().get(0).getRelativePosition().getTop() - p.getRelativePosition().getTop() + getChildHeight();

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
						currentScrollPreset = null;
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

	private BeltPreset findSelectedPreset() {
		for (OverlayControl c : getChildren()) {
			BeltPreset p = (BeltPreset) c;
			if (p.getMapsPreset().isSelected())
				return p;
		}
		return null;
	}

	private void addMissingPresets(Iterator<MapsControl> source) {
		while (source.hasNext()) {
			MapsControl sourceCtrl = source.next();
			if (sourceCtrl instanceof Preset) {
				Preset sourcePreset = (Preset) sourceCtrl;
				addChild(new BeltPreset(this, sourcePreset));
			}
		}
	}

	private void removeRemainders(Iterator<OverlayControl> target) {
		while (target.hasNext()) {
			BeltPreset targetPreset = (BeltPreset) target.next();
			target.remove();
			targetPreset.onRemoved();
		}
	}

	private void updateExistingPresets(Iterator<MapsControl> source, Iterator<OverlayControl> target) {
		while (source.hasNext() && target.hasNext()) {
			MapsControl sourceCtrl = source.next();
			if (sourceCtrl instanceof Preset) {
				Preset sourcePreset = (Preset) sourceCtrl;
				BeltPreset targetPreset = (BeltPreset) target.next();
				targetPreset.setOrigin(sourcePreset);
			}
		}
	}

	private void removeAllPresets() {
		removeAll();
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

	public void renameCurrentPreset() {
		BeltPreset p = findSelectedPreset();

		if (p != null)
			p.rename();

	}

}