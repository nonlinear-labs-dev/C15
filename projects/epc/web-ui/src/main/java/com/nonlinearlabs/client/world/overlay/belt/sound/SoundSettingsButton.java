package com.nonlinearlabs.client.world.overlay.belt.sound;

import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.Renameable;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.SoundType;
import com.nonlinearlabs.client.dataModel.setup.SetupModel;
import com.nonlinearlabs.client.presenters.EditBufferPresenterProvider;
import com.nonlinearlabs.client.useCases.EditBufferUseCases;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.RenameDialog;
import com.nonlinearlabs.client.world.overlay.ContextMenu;
import com.nonlinearlabs.client.world.overlay.ContextMenuItem;
import com.nonlinearlabs.client.world.overlay.Overlay;
import com.nonlinearlabs.client.world.overlay.OverlayLayout;
import com.nonlinearlabs.client.world.overlay.SVGImage;
import com.nonlinearlabs.client.world.overlay.html.randomizeSound.RandomizeSoundDialog;

public class SoundSettingsButton extends SVGImage {

	public class Menu extends ContextMenu {

		public Menu(OverlayLayout parent) {
			super(parent);

			EditBufferModel.SoundType currentSetting = EditBufferModel.get().soundType.getValue();

			if (currentSetting == EditBufferModel.SoundType.Single) {
				addChild(new ContextMenuItem(this, "Init") {
					@Override
					public Control click(Position eventPoint) {
						EditBufferUseCases.get().initializeSound();
						return super.click(eventPoint);
					}
				});

				addChild(new ContextMenuItem(this, "Randomize") {
					@Override
					public Control click(Position eventPoint) {
						RandomizeSoundDialog.toggle();
						return super.click(eventPoint);
					}
				});
			} else {
				addChild(new ContextMenuItem(this, "Rename Part") {
					@Override
					public Control click(Position eventPoint) {
						RenameDialog.open(new Renameable() {

							@Override
							public void setName(String newName) {
								EditBufferUseCases.get().renamePart(newName);
							}

							@Override
							public String getTitleName() {
								return EditBufferPresenterProvider.getPresenter().voiceGroup;
							}

							@Override
							public String getEntityName() {
								return "Part";
							}

							@Override
							public String getCurrentName() {
								return EditBufferPresenterProvider.getPresenter().currentPartName;
							}
						});
						return super.click(eventPoint);
					}
				});

				addChild(new ContextMenuItem(this, "Init Part") {
					@Override
					public Control click(Position eventPoint) {
						EditBufferUseCases.get().initializePart();
						return super.click(eventPoint);
					}
				});

				addChild(new ContextMenuItem(this, "Init Sound") {
					@Override
					public Control click(Position eventPoint) {
						EditBufferUseCases.get().initializeSound();
						return super.click(eventPoint);
					}
				});

				addChild(new ContextMenuItem(this, "Randomize Part") {
					@Override
					public Control click(Position eventPoint) {
						RandomizeSoundDialog.toggle();
						return super.click(eventPoint);
					}
				});

				if (currentSetting == SoundType.Split) {
					boolean syncEnabled = SetupModel.get().systemSettings.syncSplit.isTrue();
					addChild(new ContextMenuItem(this, syncEnabled ? "Unlink Splitpoints" : "Link Splitpoints") {
						@Override
						public Control click(Position eventPoint) {
							EditBufferUseCases.get().toggleSyncSplit();
							return super.click(eventPoint);
						}
					});
				}
			}
		}
	}

	public SoundSettingsButton(Control parent) {
		super(parent, "More_A_Enabled.svg");
	}

	public double getPictureHeight() {
		return getPhase(getSelectedPhase()).getImgHeight();
	}

	public double getPictureWidth() {
		return getPhase(getSelectedPhase()).getImgWidth();
	}

	@Override
	public Control mouseDown(Position pos) {
		return openContextMenu(pos);
	}

	@Override
	public Control onContextMenu(Position pos) {
		return openContextMenu(pos);
	}

	private Control openContextMenu(Position pos) {
		Overlay o = NonMaps.theMaps.getNonLinearWorld().getViewport().getOverlay();

		if (o.getContextMenu() instanceof Menu) {
			o.removeExistingContextMenus();
			return this;
		}

		pos = getPixRect().getLeftTop();
		Menu socm = new Menu(o);
		pos.moveBy(0, -socm.getDesiredHeight() - 2);
		return o.setContextMenu(pos, socm);
	}
}
