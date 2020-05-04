package com.nonlinearlabs.client.world.maps.parameters;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.Checksum;
import com.nonlinearlabs.client.world.Gray;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.maps.MapsLayout;

abstract class ModulateableValueControl extends ValueControl {

	private boolean drawCorona;
	private boolean drawThicker;

	ModulateableValueControl(MapsLayout parent, int parameterID) {
		super(parent, parameterID);
	}

	@Override
	public void draw(Context2d ctx, int invalidationMask) {
		drawCenterIndicator(ctx);
		drawBackgroundAndOutline(ctx);

		if (shouldDrawCorona()) {
			if (presenter.modulation.isModulated) {
				drawCorona(ctx, adaptValue(presenter.modulation.modulationRange.left),
						adaptValue(presenter.modulation.modulationRange.right));
			}
		}

		drawActiveArea(ctx);
	}

	protected abstract void drawCenterIndicator(Context2d ctx);

	protected abstract void drawBackgroundAndOutline(Context2d ctx);

	protected abstract void drawActiveArea(Context2d ctx);

	protected abstract void drawCorona(Context2d ctx, double left, double right);

	protected String getFontName() {
		return "'SSP-LW25'";
	}

	protected double getFontHeight() {
		return 10;
	}

	protected RGB getColorCorona() {
		return new Gray(200);
	}

	protected boolean shouldDrawThicker() {
		return drawThicker;
	}

	protected boolean shouldDrawCorona() {
		return drawCorona;
	}

	@Override
	public void doFirstLayoutPass(double levelOfDetail) {
		super.doFirstLayoutPass(levelOfDetail);

		double thickLevel = 2;
		drawThicker = levelOfDetail < thickLevel;

		double coronaLevel = 5;
		drawCorona = levelOfDetail >= coronaLevel;
	}

	@Override
	public void getStateHash(Checksum crc) {
		super.getStateHash(crc);
		crc.eat(drawThicker);
		crc.eat(drawCorona);
	}

	public double adaptValue(double v) {
		return v;
	}
}