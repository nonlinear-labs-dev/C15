package com.nonlinearlabs.client.world.maps.parameters;

import com.google.gwt.event.dom.client.KeyDownEvent;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.SoundType;
import com.nonlinearlabs.client.dataModel.editBuffer.GroupId;
import com.nonlinearlabs.client.dataModel.editBuffer.ParameterGroupModel;
import com.nonlinearlabs.client.dataModel.setup.SetupModel.SelectionAutoScroll;
import com.nonlinearlabs.client.presenters.EditBufferPresenterProvider;
import com.nonlinearlabs.client.presenters.LocalSettingsProvider;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.NonLinearWorld;
import com.nonlinearlabs.client.world.maps.LayoutResizingHorizontal;
import com.nonlinearlabs.client.world.maps.LayoutResizingVertical;
import com.nonlinearlabs.client.world.maps.NonDimension;
import com.nonlinearlabs.client.world.maps.parameters.Cabinet.Cabinet;
import com.nonlinearlabs.client.world.maps.parameters.CombFilter.CombFilter;
import com.nonlinearlabs.client.world.maps.parameters.Echo.Echo;
import com.nonlinearlabs.client.world.maps.parameters.EnvA.EnvA;
import com.nonlinearlabs.client.world.maps.parameters.EnvB.EnvB;
import com.nonlinearlabs.client.world.maps.parameters.EnvC.EnvC;
import com.nonlinearlabs.client.world.maps.parameters.FBMixer.FBMixer;
import com.nonlinearlabs.client.world.maps.parameters.Flanger.Flanger;
import com.nonlinearlabs.client.world.maps.parameters.GapFilter.GapFilter;
import com.nonlinearlabs.client.world.maps.parameters.Master.Master;
import com.nonlinearlabs.client.world.maps.parameters.OscA.OscA;
import com.nonlinearlabs.client.world.maps.parameters.OscB.OscB;
import com.nonlinearlabs.client.world.maps.parameters.OutputMixer.OutputMixer;
import com.nonlinearlabs.client.world.maps.parameters.PlayControls.PlayControls;
import com.nonlinearlabs.client.world.maps.parameters.PlayControls.MacroControls.Macros;
import com.nonlinearlabs.client.world.maps.parameters.Reverb.Reverb;
import com.nonlinearlabs.client.world.maps.parameters.SVFilter.SVFilter;
import com.nonlinearlabs.client.world.maps.parameters.Scale.Scale;
import com.nonlinearlabs.client.world.maps.parameters.ShapeA.ShapeA;
import com.nonlinearlabs.client.world.maps.parameters.ShapeB.ShapeB;
import com.nonlinearlabs.client.world.maps.parameters.MonoAndUnison.UnisonAndMono;
import com.nonlinearlabs.client.world.maps.parameters.VoiceGroupMaster.VoiceGroupMaster;
import com.nonlinearlabs.client.world.maps.presets.bank.preset.Preset;

public class ParameterEditor extends LayoutResizingVertical {

	private PlayControls playControls;
	private static ParameterEditor theEditor = null;
	private String hash = "";

	private class PlayControlsArea extends ResizingHorizontalCenteringLayout {

		private PlayControlsArea(ParameterEditor parent) {
			super(parent);
			playControls = addChild(new PlayControls(this));
		}
	}

	private class SynthParameters extends ResizingHorizontalCenteringLayout {

		private SynthParameters(ParameterEditor parent) {
			super(parent);
			addChild(new ParamEditorCol1());
			addChild(new SpacerLarge(this));
			addChild(new ParamEditorCol2());
			addChild(new SpacerLarge(this));
			addChild(new ParamEditorCol3());
			addChild(new SpacerLarge(this));
			addChild(new ParamEditorCol4());
		}

		private class ParamEditorCol1 extends LayoutResizingVertical {

			public ParamEditorCol1() {
				super(SynthParameters.this);
				addChild(new EnvA(this));
				addChild(new SpacerLarge(this));
				addChild(new EnvC(this));
				addChild(new SpacerLarge(this));
				addChild(new EnvB(this));
			}

			@Override
			public Direction getLayoutDirection() {
				return Direction.RIGHT;
			}

		}

		private class ParamEditorCol2 extends LayoutResizingVertical {

			public ParamEditorCol2() {
				super(SynthParameters.this);
				addChild(new OscA(this));
				addChild(new SpacerLarge(this));
				addChild(new ShapeA(this));
				addChild(new SpacerLarge(this));
				addChild(new ShapeB(this));
				addChild(new SpacerLarge(this));
				addChild(new OscB(this));
			}
		}

		private class ParamEditorCol3 extends LayoutResizingVertical {

			public ParamEditorCol3() {
				super(SynthParameters.this);
				addChild(new FBMixer(this));
				addChild(new SpacerLarge(this));
				addChild(new CombFilter(this));
				addChild(new SpacerLarge(this));
				addChild(new SVFilter(this));
				addChild(new SpacerLarge(this));
				addChild(new OutputMixer(this));
			}
		}

		private class ParamEditorCol4 extends LayoutResizingVertical {

			private class CabinetAndGapFilter extends LayoutResizingHorizontal {

				public CabinetAndGapFilter() {
					super(SynthParameters.this);

					addChild(new Cabinet(this));
					addChild(new SpacerLarge(this));
					addChild(new GapFilter(this));
				}
			}

			private class EchoAndReverb extends LayoutResizingHorizontal {

				public EchoAndReverb() {
					super(SynthParameters.this);

					addChild(new Echo(this));
					addChild(new SpacerLarge(this));
					addChild(new Reverb(this));
				}
			}

			private class Masters extends LayoutResizingHorizontal {

				private class VoiceGroupMasterAndSpacer extends LayoutResizingHorizontal {
					VoiceGroupMasterAndSpacer(Masters parent) {
						super(parent);
						addChild(new VoiceGroupMaster(this));
						addChild(new SpacerLarge(this));

						EditBufferModel.get().soundType.onChange(v -> {
							requestLayout();
							return true;
						});
					}

					@Override
					public void doFirstLayoutPass(double levelOfDetail) {
						if (EditBufferModel.get().soundType.getValue() == SoundType.Single) {
							setNonSize(new NonDimension(0, 0));
						} else {
							super.doFirstLayoutPass(levelOfDetail);
						}
					}
				}

				public Masters() {
					super(SynthParameters.this);

					addChild(new UnisonAndMono(this));
					addChild(new SpacerLarge(this));

					addChild(new VoiceGroupMasterAndSpacer(this));

					addChild(new Master(this));
					addChild(new SpacerLarge(this));
					addChild(new Scale(this));
				}
			}

			private ParamEditorCol4() {
				super(SynthParameters.this);

				addChild(new Flanger(this));
				addChild(new SpacerLarge(this));
				addChild(new CabinetAndGapFilter());

				addChild(new SpacerLarge(this));
				addChild(new EchoAndReverb());
				addChild(new SpacerLarge(this) {
					@Override
					protected double getBasicWidth() {
						return super.getBasicWidth() * 8;
					}
				});

				addChild(new Masters());
			}

			@Override
			public Direction getLayoutDirection() {
				return Direction.LEFT;
			}
		}
	}

	public ParameterEditor(NonLinearWorld parent) {
		super(parent);

		assert (theEditor == null);
		theEditor = this;

		addChild(new SpacerLarge(this) {
			@Override
			protected double getBasicHeight() {
				return 10;
			}
		});

		addChild(new PlayControlsArea(this));
		addChild(new SpacerLarge(this));
		addChild(new SpacerLarge(this));
		addChild(new SynthParameters(this));

		EditBufferModel.get().selectedParameter.onChange(i -> {
			boolean autoScroll = LocalSettingsProvider.get().getSettings().selectionAutoScroll
					.isOneOf(SelectionAutoScroll.parameter, SelectionAutoScroll.parameter_and_preset);

			ParameterGroupModel group = EditBufferModel.get().getAnyParameter(i).group;
			if (autoScroll && group != null) {
				scrollToSelectedParameterGroup(group.id);
			}
			invalidate(INVALIDATION_FLAG_UI_CHANGED);
			return true;
		});
	}

	public Control onKey(final KeyDownEvent event) {

		int keyCode = event.getNativeEvent().getKeyCode();

		if (keyCode == com.google.gwt.event.dom.client.KeyCodes.KEY_E
				&& getNonMaps().getNonLinearWorld().isCtrlDown()) {
			toggleVisibility();
			return this;
		}
		return null;
	}

	public static ParameterEditor get() {
		return theEditor;
	}

	private void scrollToSelectedParameterGroup(GroupId id) {
		if (id.getName() == "CS")
			id = new GroupId("MCM", id.getVoiceGroup());

		if(id.getName() == "Split")
			id = new GroupId("Part", EditBufferModel.get().voiceGroup.getValue());

		ParameterGroup p = findParameterGroup(id);

		if (!p.isVisible())
			p.scrollToMakeFullyVisible();
	}

	public ParameterGroup findParameterGroup(final GroupId groupID) {
		Control found = recurseChildren(new ControlFinder() {

			@Override
			public boolean onWayDownFound(Control child) {
				if (child instanceof ParameterGroup) {
					ParameterGroup i = (ParameterGroup) child;
					if (i.getName().equals(groupID.getName()))
						return true;
				}
				return false;
			}
		});

		return (ParameterGroup) found;
	}

	public String getLoadedPresetUUID() {
		return EditBufferPresenterProvider.getPresenter().loadedPresetUUID;
	}

	public String getLoadedPresetName() {
		return "Init";
	}

	public Macros getMacroControls() {
		return playControls.getMacroControls();
	}

	public PlayControls getPlayControls() {
		return playControls;
	}

	public boolean isEditorVisible() {
		return isVisible();
	}

	public void toggleVisibility() {
		setVisible(!isIntendedVisible());
	}

	public String getLoadedPresetInfo() {
		String uuid = EditBufferPresenterProvider.getPresenter().loadedPresetUUID;
		Preset p = NonMaps.get().getNonLinearWorld().getPresetManager().findPreset(uuid);

		if (p != null)
			return p.getAttribute("Comment");

		return "";
	}

	public String getHash() {
		return hash;
	}

	public boolean isAnyParameterLocked() {
		Control c = recurseChildren(new ControlFinder() {
			@Override
			public boolean onWayDownFound(Control ctrl) {
				if (ctrl instanceof Parameter) {
					Parameter p = (Parameter) ctrl;
					if (p.isLocked()) {
						return true;
					}
				}
				return false;
			}
		});

		return c != null;
	}

	public Parameter findParameter(int id) {
		return (Parameter) recurseChildren(new ControlFinder() {
			@Override
			public boolean onWayDownFound(Control ctrl) {
				if (ctrl instanceof Parameter) {
					Parameter p = (Parameter) ctrl;
					if (p.getParameterNumber() == id) {
						return true;
					}
				}
				return false;
			}
		});
	}

	public Parameter getSelectedParameter() {
		return findParameter(EditBufferModel.get().selectedParameter.getValue());
	}

}
