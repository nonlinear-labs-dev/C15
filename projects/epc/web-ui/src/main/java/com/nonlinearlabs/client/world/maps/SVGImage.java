package com.nonlinearlabs.client.world.maps;

import com.google.gwt.canvas.dom.client.Context2d;
import com.google.gwt.dom.client.ImageElement;
import com.nonlinearlabs.client.Checksum;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.maps.RenderedSVGImageStore.SVGRenderHandler;
import com.nonlinearlabs.client.world.maps.parameters.ZoomReactingControl;

public abstract class SVGImage extends ZoomReactingControl implements SVGRenderHandler {
	private String[] imageNames;
	private ImageElement img;

	public SVGImage(MapsLayout parent, String... imageNames) {
		super(parent);

		this.imageNames = imageNames;

		for (int i = 0; i < this.imageNames.length; i++) {
			this.imageNames[i] = "images/" + this.imageNames[i];
		}
	}

	abstract public int getSelectedPhase();

	@Override
	public void getStateHash(Checksum crc) {
		super.getStateHash(crc);
		if (img != null)
			crc.eat(img.hashCode());
	}

	private void getSelectedImage() {
		int face = getSelectedPhase();

		if (face >= 0 && face < imageNames.length) {
			Rect r = getPixRect();
			ImageElement i = RenderedSVGImageStore.get().render(imageNames[face], (int) Math.ceil(r.getWidth()),
					(int) Math.ceil(r.getHeight()), this);

			if (i != null)
				img = i;
		}
	}

	@Override
	public void onSVGRendered(ImageElement img) {
		if (this.img != img) {
			this.img = img;
			invalidate(INVALIDATION_FLAG_UI_CHANGED);
		}
	}

	@Override
	public void draw(Context2d ctx, Context2d overlay, Context2d menus, int invalidationMask) {
		getSelectedImage();

		if (img != null) {
			Rect pix = getBitmapRect();

			double h = img.getHeight();
			double w = img.getWidth();

			if (h != 0 && w != 0) {

				double minDim = Math.min(pix.getWidth(), pix.getHeight());
				double xOffset = 0;
				double yOffset = 0;

				if (minDim == pix.getWidth()) {
					h *= pix.getWidth() / (double) w;
					w = pix.getWidth();
					yOffset = (pix.getHeight() - h) / 2;
				} else {
					w *= pix.getHeight() / (double) h;
					h = pix.getHeight();
					xOffset = (pix.getWidth() - w) / 2;
				}

				if (w > 0 && h > 0) {
					ctx.drawImage(img, pix.getLeft() + xOffset, pix.getTop() + yOffset, w, h);
				}
			}
		}
	}

	protected Rect getBitmapRect() {
		Rect r = getPixRect().copy();
		r.applyPadding(toXPixels(getLeftPadding()), toYPixels(getTopPadding()), toXPixels(getRightPadding()),
				toYPixels(getBottomPadding()));
		return r;
	}

	protected double getPadding() {
		return 0;
	}

	protected double getXPadding() {
		return getPadding();
	}

	protected double getYPadding() {
		return getPadding();
	}

	protected double getLeftPadding() {
		return getXPadding();
	}

	protected double getRightPadding() {
		return getXPadding();
	}

	protected double getTopPadding() {
		return getYPadding();
	}

	protected double getBottomPadding() {
		return getYPadding();
	}

}
