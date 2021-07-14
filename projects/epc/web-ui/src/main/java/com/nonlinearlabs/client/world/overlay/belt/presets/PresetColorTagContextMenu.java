package com.nonlinearlabs.client.world.overlay.belt.presets;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.Millimeter;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.maps.presets.PresetManager;
import com.nonlinearlabs.client.world.maps.presets.bank.preset.ColorTag;
import com.nonlinearlabs.client.world.maps.presets.bank.preset.Preset;
import com.nonlinearlabs.client.world.overlay.ContextMenu;
import com.nonlinearlabs.client.world.overlay.OverlayLayout;
import com.nonlinearlabs.client.world.overlay.PresetContextMenuItem;

public class PresetColorTagContextMenu extends ContextMenu {

	private final Preset preset;

	private void setColor(ColorTag.Color color) {
		final PresetManager pm = preset.getParent().getParent();
		final boolean hasMultipleSelection = pm.hasMultiplePresetSelection();

		if (hasMultipleSelection) {
			for (String uuid : pm.getMultiSelection().getSelectedPresets()) {
				Preset p = pm.findPreset(uuid);
				NonMaps.get().getServerProxy().setPresetAttribute(p, "color", color.toString());
			}
		} else {
			NonMaps.get().getServerProxy().setPresetAttribute(preset, "color", color.toString());
		}
	}

	public PresetColorTagContextMenu(OverlayLayout parent, final Preset p) {
		super(parent);
		this.preset = p;

		for (ColorTag.Color c : ColorTag.Color.values()) {
			String name = c.toString();
			name = name.substring(0, 1).toUpperCase() + name.substring(1);
			addChild(new PresetContextMenuItem((ContextMenu) this, name) {
				@Override
				public Control click(Position eventPoint) {
					setColor(c);
					NonMaps.get().getNonLinearWorld().getViewport().getOverlay().removeExistingContextMenus();
					return super.click(eventPoint);
				}

				@Override
				public void draw(Context2d ctx, int invalidationMask) {
					super.draw(ctx, invalidationMask);
					Rect r = this.getPixRect().copy();
					r.setWidth(getColorTagWidth());
					r.reduceHeightBy(r.getHeight() / 1.9);
					r.moveBy(-3, 0);
					r.fill(ctx, c.toRGB());
				}

				public double getColorTagWidth() {
					return Millimeter.toPixels(3.2);
				}

				@Override
				public double getDesiredHeight() {
					return Millimeter.toPixels(10);
				}

				@Override
				public double getDesiredWidth() {
					return super.getDesiredWidth() + getColorTagWidth();
				}

				@Override
				public double getLeftPadding() {
					return super.getLeftPadding() + getColorTagWidth();
				}
			});
		}
	}
}
