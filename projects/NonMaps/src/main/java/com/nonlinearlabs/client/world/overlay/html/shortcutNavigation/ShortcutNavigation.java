package com.nonlinearlabs.client.world.overlay.html.shortcutNavigation;

import com.google.gwt.core.client.GWT;
import com.google.gwt.event.dom.client.KeyCodes;
import com.google.gwt.uibinder.client.UiBinder;
import com.google.gwt.uibinder.client.UiField;
import com.google.gwt.user.client.ui.Composite;
import com.google.gwt.user.client.ui.HTMLPanel;
import com.google.gwt.user.client.ui.ListBox;
import com.google.gwt.user.client.ui.TextArea;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.world.NavigationShortcuts;

public class ShortcutNavigation extends Composite {
	interface ShortcutNavigationUiBinder extends UiBinder<HTMLPanel, ShortcutNavigation> {
	}

	private static ShortcutNavigationUiBinder ourUiBinder = GWT.create(ShortcutNavigationUiBinder.class);

	@UiField
	ListBox proposals;

	@UiField
	TextArea query;

	NavigationShortcuts shortcuts = new NavigationShortcuts();

	public ShortcutNavigation(ShortcutNavigationDialog d) {
		initWidget(ourUiBinder.createAndBindUi(this));
		NonMaps.get().getNonLinearWorld().fill(shortcuts);

		query.addKeyUpHandler((event) -> {
			if (event.getNativeKeyCode() == KeyCodes.KEY_DOWN) {
				int s = proposals.getSelectedIndex() + 1;
				if (s < proposals.getItemCount())
					proposals.setSelectedIndex(s);

			} else if (event.getNativeKeyCode() == KeyCodes.KEY_UP) {
				int s = proposals.getSelectedIndex() - 1;
				if (s >= 0 && s < proposals.getItemCount())
					proposals.setSelectedIndex(s);

			} else if (event.getNativeKeyCode() == KeyCodes.KEY_ENTER) {
				shortcuts.execute(Integer.parseInt(proposals.getSelectedValue()));
				d.commit();
			} else {
				refreshProposals(query.getValue());
			}
		});

	}

	private void refreshProposals(String q) {
		String selValue = proposals.getSelectedValue();

		proposals.clear();

		for (NavigationShortcuts.Entry e : shortcuts.query(q)) {
			String id = Integer.toString(e.id);
			proposals.addItem(e.path, id);
			if (selValue != null && !selValue.isEmpty() && selValue.equals(id)) {
				proposals.setSelectedIndex(proposals.getItemCount() - 1);
			}
		}
	}

	public void setFocus() {
		query.setFocus(true);
	}

}
