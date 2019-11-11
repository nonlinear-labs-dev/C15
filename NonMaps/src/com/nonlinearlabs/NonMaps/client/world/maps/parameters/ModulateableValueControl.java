package com.nonlinearlabs.NonMaps.client.world.maps.parameters;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.NonMaps.client.Checksum;
import com.nonlinearlabs.NonMaps.client.tools.NLMath;
import com.nonlinearlabs.NonMaps.client.world.Gray;
import com.nonlinearlabs.NonMaps.client.world.RGB;
import com.nonlinearlabs.NonMaps.client.world.maps.MapsLayout;

abstract class ModulateableValueControl extends ValueControl {

	private boolean drawCorona;
	private boolean drawThicker;

	ModulateableValueControl(MapsLayout parent) {
		super(parent);
	}

	@Override
	public void draw(Context2d ctx, int invalidationMask) {
		drawCenterIndicator(ctx);
		drawBackgroundAndOutline(ctx);

		if (shouldDrawCorona() && getParentParameterLayout() instanceof ModulatableParameter) {

			ModulatableParameter p = (ModulatableParameter) getParentParameterLayout();
			Parameter mc = getNonMaps().getNonLinearWorld().getParameterEditor().getMacroControls()
					.getControl(p.getModulationSource());

			if (mc != null) {
				double modAmount = p.getModulationAmount().getQuantizedClipped();
				double value = p.getValue().getQuantizedUnClipped();
				boolean isBiPolar = p.isBiPolar();

				double modLeft = 0;
				double modRight = 0;

				if (isBiPolar)
					modAmount *= 2;

				modLeft = value - modAmount * mc.getValue().getQuantizedClipped();
				modRight = modLeft + modAmount;

				if (modRight < modLeft) {
					double tmp = modLeft;
					modLeft = modRight;
					modRight = tmp;
				}

				if (isBiPolar) {
					modLeft = NLMath.clamp(modLeft, -1.0, 1.0);
					modRight = NLMath.clamp(modRight, -1.0, 1.0);
				} else {
					modLeft = NLMath.clamp(modLeft, 0.0, 1.0);
					modRight = NLMath.clamp(modRight, 0.0, 1.0);
				}

				drawCorona(ctx, adaptValue(modLeft), adaptValue(modRight));
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

		if (getParentParameterLayout() instanceof ModulatableParameter) {
			ModulatableParameter p = (ModulatableParameter) getParentParameterLayout();

			double modAmount = p.getModulationAmount().getQuantizedClipped();
			double modSrcValue = 0;
			Parameter mc = getNonMaps().getNonLinearWorld().getParameterEditor().getMacroControls()
					.getControl(p.getModulationSource());

			if (mc != null)
				modSrcValue = mc.getValue().getQuantizedClipped();

			crc.eat(modAmount);
			crc.eat(modSrcValue);
			crc.eat(getModSourceString());
			crc.eat(drawCorona);
		}

		crc.eat(getValue());
		crc.eat(getModAmount());
	}

	private double getModAmount() {
		if (getParentParameterLayout() instanceof ModulatableParameter) {
			ModulatableParameter p = (ModulatableParameter) getParentParameterLayout();
			return p.getModulationAmount().getQuantizedClipped();
		}
		return 0;
	}

	private String getModSourceString() {
		if (getParentParameterLayout() instanceof ModulatableParameter) {
			ModulatableParameter p = (ModulatableParameter) getParentParameterLayout();
			return p.getModSourceString();
		}
		return "--";
	}

	public double adaptValue(double v) {
		return v;
	}
}