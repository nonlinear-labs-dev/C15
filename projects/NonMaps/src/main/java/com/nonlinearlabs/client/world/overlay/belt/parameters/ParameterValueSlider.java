package com.nonlinearlabs.client.world.overlay.belt.parameters;
import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.Millimeter;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.presenters.EditBufferPresenterProvider;
import com.nonlinearlabs.client.presenters.ParameterPresenter;
import com.nonlinearlabs.client.world.Gray;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.overlay.OverlayControl;

public class ParameterValueSlider extends OverlayControl {

	public ParameterValueSlider(Sliders parent) {
		super(parent);
	}

	@Override
	public Sliders getParent() {
		return (Sliders) super.getParent();
	}

	@Override
	public void draw(Context2d ctx, int invalidationMask) {
		ParameterPresenter parameter = EditBufferPresenterProvider.getPresenter().selectedParameter;
		double value = parameter.controlPosition;
		boolean isBiPolar = parameter.bipolar;

		if (!parameter.drawHandleOnly) {
			getPixRect().fillAndStroke(ctx, new Gray(25), 1, new Gray(0));

			if (isBiPolar)
				drawCenterMark(ctx);

			if(parameter.fillFromRightEnabled)
				drawIndicatorFromRight(ctx, value);
			else
				drawIndicator(ctx, value, isBiPolar);

			drawHandle(ctx, value, isBiPolar);
		} else {

			Rect background = getPixRect().copy();
			double rounding = background.getHeight() / 2;
			background.applyPadding(-rounding, 0, -rounding, 0);
			background.drawRoundedArea(ctx, rounding, 1, new Gray(25), new Gray(0));

			if (isBiPolar)
				drawCenterMark(ctx);
			drawRoundHandle(ctx, value, isBiPolar);
			drawReturnIndicators(ctx, parameter);

		}
	}

	protected void drawIndicatorFromRight(Context2d ctx, double value) {
	
		Rect indicatorRect = getPixRect().copy();

		double width = indicatorRect.getWidth() - indicatorRect.getWidth() * value; 

		indicatorRect.setLeft(indicatorRect.getRight() - width);
		indicatorRect.setWidth(width);
		indicatorRect.reduceHeightBy(2);
		indicatorRect.reduceWidthBy(2);
		indicatorRect.fill(ctx, getIndicatorColor());
		
	}

	protected void drawReturnIndicators(Context2d ctx, ParameterPresenter parameter) {
		Rect pixRect = getPixRect().copy();

			double oneMM = Millimeter.toPixels(1);
			double top = pixRect.getTop() - oneMM;
			double bottom = pixRect.getBottom() + oneMM;
			
		if (parameter.drawCenterReturnIndicator) 
			drawReturnArrow(ctx, pixRect.getCenterPoint().getX(), top, bottom);
		else if(parameter.drawZeroReturnIndicator)
			drawReturnArrow(ctx, pixRect.getLeft(), top, bottom);
	}

	protected void drawReturnArrow(Context2d ctx, double x, double top, double bottom) {
		double strength = NonMaps.mmToPixels(1.5);
		double halfStrength = strength / 2;

		ctx.setLineWidth(1);
		ctx.setFillStyle(getColorIndicator().toString());
		ctx.setStrokeStyle(getColorObjectContour().toString());

		ctx.beginPath();
		ctx.moveTo(x - strength, top - strength);
		ctx.lineTo(x + strength, top - strength);
		ctx.lineTo(x, top + halfStrength);
		ctx.closePath();

		ctx.fill();
		ctx.stroke();

		ctx.beginPath();
		ctx.moveTo(x - strength, bottom + strength);
		ctx.lineTo(x + strength, bottom + strength);
		ctx.lineTo(x, bottom - halfStrength);
		ctx.closePath();

		ctx.fill();
		ctx.stroke();
	}

	private void drawIndicator(Context2d ctx, double value, boolean isBiPolar) {
		Rect indicatorRect = getPixRect().copy();

		if (isBiPolar) {
			indicatorRect.setLeft(indicatorRect.getCenterPoint().getX());
			indicatorRect.setWidth(indicatorRect.getWidth() / 2);
		}

		indicatorRect.setWidth(indicatorRect.getWidth() * value);
		indicatorRect.reduceHeightBy(2);
		indicatorRect.reduceWidthBy(2);
		indicatorRect.fill(ctx, getIndicatorColor());
	}

	private RGB getIndicatorColor() {
		if (!getParent().getParent().isModulationAssigned())
			return new Gray(230);

		return getParent().getParent().isOneOf(BeltParameterLayout.Mode.unmodulateableParameter,
				BeltParameterLayout.Mode.modulateableParameter, BeltParameterLayout.Mode.paramValue,
				BeltParameterLayout.Mode.mcSource) ? new Gray(230) : new Gray(77);
	}

	private void drawHandle(Context2d ctx, double value, boolean isBipolar) {
		Rect sliderRect = getPixRect().copy();
		RGB color = getRoundHandleColor(value);
		double handleWidth = NonMaps.mmToPixels(1);
		sliderRect.reduceWidthBy(handleWidth);
		sliderRect.reduceHeightBy(2);

		Rect r = new Rect(0, sliderRect.getTop(), handleWidth, sliderRect.getHeight());

		if (isBipolar) {
			r.setLeft(sliderRect.getCenterPoint().getX() + sliderRect.getWidth() / 2 * value - handleWidth / 2);
		} else {
			r.setLeft(sliderRect.getLeft() + sliderRect.getWidth() * value - handleWidth / 2);
		}

		r.fill(ctx, color);
	}

	private void drawRoundHandle(Context2d ctx, double value, boolean isBipolar) {
		Rect sliderRect = getPixRect().copy();
		RGB color = getRoundHandleColor(value);

		double x = 0;
		double radius = 1.5 * sliderRect.getHeight() / 2;

		if (isBipolar) {
			x = sliderRect.getCenterPoint().getX() + sliderRect.getWidth() / 2 * value;
		} else {
			x = sliderRect.getLeft() + sliderRect.getWidth() * value;
		}

		ctx.setStrokeStyle(getColorObjectContour().toString());
		ctx.setLineWidth(1);
		ctx.setFillStyle(color.toString());
		ctx.beginPath();
		ctx.arc(x, sliderRect.getCenterPoint().getY(), radius, 0, 2 * Math.PI);
		ctx.fill();
		ctx.stroke();

	}

	protected RGB getRoundHandleColor(double value) {
		return shouldHandleBeDimmed(value) ? getColorObjectContour() : getColorIndicator();
	}

	protected boolean shouldHandleBeDimmed(double value) {
		// return Math.abs(value) < 0.001 && getParameter().dimHandleAtDefaultValue();
		// todo
		return false;
	}

	private void drawCenterMark(Context2d ctx) {
		Rect r = getPixRect();
		Position c = r.getCenterPoint();
		double markThickness = NonMaps.mmToPixels(2);
		Rect b = new Rect(c.getX() - markThickness / 2, r.getTop(), markThickness, r.getHeight());
		b.fill(ctx, getColorObjectContour());
	}
}
