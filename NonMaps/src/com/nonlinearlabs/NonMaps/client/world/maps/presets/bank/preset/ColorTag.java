package com.nonlinearlabs.NonMaps.client.world.maps.presets.bank.preset;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.NonMaps.client.world.Position;
import com.nonlinearlabs.NonMaps.client.world.RGB;
import com.nonlinearlabs.NonMaps.client.world.Rect;
import com.nonlinearlabs.NonMaps.client.world.maps.MapsLayout;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.ZoomReactingControl;

public class ColorTag extends ZoomReactingControl {

	private Preset preset = null;
	
	public ColorTag(MapsLayout parent) {
		super(parent);
		this.preset = getParent();
	}
	
	protected RGB calcColor() {
	
		switch(this.preset.getAttribute("color"))
		{
			case "green":
				return new RGB(0, 255, 0);
			case "blue":
				return new RGB(0, 0, 255);
			case "yellow":
				return new RGB(255, 255, 0);
			case "orange":
				return new RGB(255, 126, 0);
			case "purple":
				return new RGB(255, 0, 255);
			case "red":
				return new RGB(255, 0, 0);
			default:
				return null;
		}
	}
	
	@Override
	public Preset getParent() {
		return (Preset)super.getParent();
	}
	
	@Override
	protected double getBasicWidth() {
		return 3;
	}

	@Override
	protected double getBasicHeight() {
		return 15;
	}

	@Override
	protected double getMinHeight() {
		return 0;
	}
		
	@Override
	public void draw(Context2d ctx, int invalidationMask) {
		RGB color = calcColor();
		if(color == null)
			return;
		Rect pixRect = getPixRect();
		ctx.setFillStyle(color.toString());
		Position center = pixRect.getCenterPoint();
		ctx.fillRect(center.getX(), center.getY(), pixRect.getWidth(), pixRect.getHeight());
	}
}
