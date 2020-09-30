package com.nonlinearlabs.client.world.overlay.belt.presets;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Gray;
import com.nonlinearlabs.client.world.IPreset;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.RGBA;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.maps.presets.bank.Bank;
import com.nonlinearlabs.client.world.overlay.DragProxy;
import com.nonlinearlabs.client.world.overlay.Label;
import com.nonlinearlabs.client.world.overlay.OverlayLayout;
import com.nonlinearlabs.client.world.overlay.belt.EditBufferDraggingButton;

public class EmptyBeltPreset extends OverlayLayout {

	private class EmptyLabel extends Label {

		private boolean dropping = false;
		private Bank bank;

		public EmptyLabel(OverlayLayout parent, Bank bank) {
			super(parent);
			this.bank = bank;
		}

		@Override
		public String getDrawText(Context2d ctx) {
			return "- empty -";
		}

		@Override
		public void draw(Context2d ctx, Context2d overlay, int invalidationMask) {
			Rect rect = getPixRect();

			if (dropping) {
				rect.fill(ctx, new RGBA(255, 0, 0, 0.75));
			} else {
				rect.fill(ctx, new Gray(25));
			}

			super.draw(ctx, overlay, invalidationMask);
		}

		@Override
		public RGB getColorFont() {
			return new Gray(128);
		}

		@Override
		public Control drop(Position pos, DragProxy dragProxy) {
			if (dragProxy.getOrigin() instanceof IPreset) {
				IPreset p = (IPreset) dragProxy.getOrigin();
				getNonMaps().getServerProxy().appendPreset(p.getUUID(), this.bank.getUUID());
				return this;
			} else if (dragProxy.getOrigin() instanceof EditBufferDraggingButton) {
				getNonMaps().getServerProxy().appendEditBuffer(this.bank);
				return this;
			}

			return null;
		}

		@Override
		public Control drag(Position pos, DragProxy dragProxy) {
			if (dragProxy.getOrigin() instanceof IPreset || dragProxy.getOrigin() instanceof EditBufferDraggingButton) {
				dropping = true;
				return this;
			}
			return super.drag(pos, dragProxy);
		}

		@Override
		public Control mouseDrag(Position oldPoint, Position newPoint, boolean fine) {
			return super.mouseDrag(oldPoint, newPoint, fine);
		}

		@Override
		public void dragLeave() {
			dropping = false;
			invalidate(INVALIDATION_FLAG_UI_CHANGED);
		}

		@Override
		public void doLayout(double x, double y, double w, double h) {
			super.doLayout(x, y, w, h);
		}

	}

	Label emptyLabel;

	public EmptyBeltPreset(OverlayLayout parent, Bank bank) {
		super(parent);
		emptyLabel = addChild(new EmptyLabel(this, bank));
	}

	@Override
	public void doLayout(double x, double y, double w, double h) {
		super.doLayout(x, y, w, h);
		emptyLabel.doLayout(0, 0, w, h);
	}

}