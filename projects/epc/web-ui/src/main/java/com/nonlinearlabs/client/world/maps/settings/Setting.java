package com.nonlinearlabs.client.world.maps.settings;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.maps.IContextMenu;
import com.nonlinearlabs.client.world.maps.Label;
import com.nonlinearlabs.client.world.maps.LayoutResizingHorizontal;
import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.NonPosition;

abstract public class Setting extends LayoutResizingHorizontal {

	private class ValueButton extends Label {
		private ValueButton(MapsLayout parent, String text) {
			super(parent, text);
		}

		@Override
		public Control mouseDown(Position eventPoint) {
			return onValueButtonMouseDown(eventPoint);
		}
	}

	private Label title;
	private Label value;

	protected class SettingsLabel extends Label {
		public SettingsLabel(MapsLayout parent, String text) {
			super(parent, text);
		}

		@Override
		protected int getNonWidth() {
			return 280;
		}
	};

	protected Setting(MapsLayout parent, String title, String value) {
		super(parent);

		this.title = addChild(new SettingsLabel(this, title));
		this.value = addChild(new ValueButton(this, value));
	}

	public Control onValueButtonMouseDown(Position eventPoint) {
		return (Control) createMenu(toNonPosition(eventPoint));
	}

	public void setTitle(String title) {
		this.title.setText(title);
	}

	public void setCurrentValue(String value) {
		this.value.setText(value);
	}

	public String getCurrentValue() {
		return this.value.getText();
	}

	@Override
	public void draw(Context2d ctx, Context2d overlay, Context2d menus, int invalidationMask) {
		super.draw(ctx, overlay, menus, invalidationMask);
		getPixRect().stroke(ctx, toXPixels(1), getParent().getColorFont());
	}

	public abstract IContextMenu createMenu(NonPosition pos);

	public abstract void setDefault();
}
