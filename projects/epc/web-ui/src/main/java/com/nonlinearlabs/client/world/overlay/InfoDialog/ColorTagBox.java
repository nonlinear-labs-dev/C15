package com.nonlinearlabs.client.world.overlay.InfoDialog;

import com.google.gwt.event.dom.client.ClickEvent;
import com.google.gwt.event.dom.client.ClickHandler;
import com.google.gwt.user.client.ui.Button;
import com.google.gwt.user.client.ui.HTMLPanel;
import com.nonlinearlabs.client.dataModel.presetManager.Preset.Color;

public abstract class ColorTagBox {

	HTMLPanel panel;
	Button buttons[];

	public ColorTagBox() {
		buttons = new Button[7];
		panel = new HTMLPanel("div", "");
		for (Color c : Color.values()) {
			panel.add(addColorButton(c));
		}
		panel.getElement().addClassName("tag-div");
	}

	public HTMLPanel getHTML() {
		return panel;
	}

	private Button addColorButton(Color c) {
		Button b = new Button();
		if (c == Color.none)
			b.setHTML("<div style=\"font-size: smaller;\">✕</div>");
		b.getElement().addClassName("colortaggingfield-" + c.name());
		b.getElement().addClassName("colortaggingfield-template");
		b.addClickHandler(new ClickHandler() {
			@Override
			public void onClick(ClickEvent event) {
				onBoxClick(c);
				updateCurrentHighlight(c);
			}
		});
		buttons[c.ordinal()] = b;
		return b;
	}

	public void updateCurrentHighlight(Color current) {
		int activeColorIndex = current.ordinal();
		for (int i = 0; i < buttons.length; i++) {
			Button b = buttons[i];

			if (activeColorIndex != i)
				b.getElement().removeClassName("colortag-current");
			else
				b.getElement().addClassName("colortag-current");
		}
	}

	public abstract void onBoxClick(Color c);
}
