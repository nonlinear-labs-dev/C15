package com.nonlinearlabs.client.world.maps.parameters;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.world.Gray;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.Rect;

public class ModulateableParameterName extends LabelLarge {

	private String name = null;

	public ModulateableParameterName(ModulatableParameter p) {
		super(p);
	}

	public ModulateableParameterName(ModulatableParameter p, String name) {
		super(p);
		this.name = name;
	}

	@Override
	public Parameter getParent() {
		return (Parameter) super.getParent();
	}

	@Override
	public void draw(Context2d ctx, Context2d overlay, Context2d menus, int invalidationMask) {
		getPixRect().drawRoundedRect(ctx, getBackgroundRoundings(), toXPixels(4), 0, getNameBackgroundColor(), null);
		super.draw(ctx, overlay, menus, invalidationMask);
	}

	@Override
	protected double getMinHeight() {
		return 0;
	}

	private RGB getNameBackgroundColor() {
		return getParent().isSelected() ? getColorObjectContourSelected() : new Gray(87);
	}

	private int getBackgroundRoundings() {
		return getParent().getBackgroundRoundings() & Rect.ROUNDING_TOP;
	}

	@Override
	public RGB getColorFont() {
		return getParent().isSelected() ? getColorSliderBackground() : super.getColorFont();
	}

	@Override
	protected double getBasicWidth() {
		return 80;
	}

	protected String getDisplayText() {
		if (name != null)
			return name;

		return getParent().presenter.shortName;
	}

}
