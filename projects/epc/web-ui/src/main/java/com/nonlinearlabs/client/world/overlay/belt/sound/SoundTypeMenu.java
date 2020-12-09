package com.nonlinearlabs.client.world.overlay.belt.sound;

import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel;
import com.nonlinearlabs.client.useCases.EditBufferUseCases;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.overlay.ContextMenu;
import com.nonlinearlabs.client.world.overlay.ContextMenuItem;
import com.nonlinearlabs.client.world.overlay.Overlay;
import com.nonlinearlabs.client.world.overlay.OverlayLayout;
import com.nonlinearlabs.client.world.overlay.SVGImage;

public class SoundTypeMenu extends SVGImage {

	public class SoundTypeMenuContextMenu extends ContextMenu {

		private void addSingleChild() {
			addChild(new ContextMenuItem(this, "Convert to Single") {
				@Override
				public Control click(Position eventPoint) {
					EditBufferUseCases.get().convertToSingleSound();
					return super.click(eventPoint);
				}
			});
		} 

		private void addLayerChild() {
			addChild(new ContextMenuItem(this, "Convert to Layer") {
				@Override
				public Control click(Position eventPoint) {
					EditBufferUseCases.get().convertToLayerSound();
					return super.click(eventPoint);
				}
			});
		}

		private void addSplitChild() {
			addChild(new ContextMenuItem(this, "Convert to Split") {
				@Override
				public Control click(Position eventPoint) {
					EditBufferUseCases.get().convertToSplitSound();
					return super.click(eventPoint);
				}
			});
		}

		public SoundTypeMenuContextMenu(OverlayLayout parent) {
			super(parent);

			EditBufferModel.SoundType currentSetting = EditBufferModel.get().soundType.getValue();


			if (currentSetting == EditBufferModel.SoundType.Single) {
				addLayerChild();
				addSplitChild();
			}

			if (currentSetting == EditBufferModel.SoundType.Layer) {
				addSingleChild();
				addSplitChild();
			}

			if (currentSetting == EditBufferModel.SoundType.Split) {
				addSingleChild();
				addLayerChild();
			}
		}
	}

	public SoundTypeMenu(OverlayLayout parent) {
		super(parent, "Menu_Single_Enabled_L.svg", "Menu_Split_Enabled_L.svg", "Menu_Layer_Enabled_L.svg");
	}

	@Override
	public int getSelectedPhase() {
		switch (EditBufferModel.get().soundType.getValue()) {
		case Single:
			return 0;
		case Split:
			return 1;
		case Layer:
			return 2;
		default:
			return 0;
		}
	};

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

		if (o.getContextMenu() instanceof SoundTypeMenuContextMenu) {
			o.removeExistingContextMenus();
			return this;
		}

		pos = getPixRect().getLeftTop();
		SoundTypeMenuContextMenu socm = new SoundTypeMenuContextMenu(o);
		pos.moveBy(0, -socm.getDesiredHeight() - 2);
		return o.setContextMenu(pos, socm);
	}

	public double getPictureHeight() {
		return getPhase(getSelectedPhase()).getImgHeight();
	}

	public double getPictureWidth() {
		return getPhase(getSelectedPhase()).getImgWidth();
	}
}
