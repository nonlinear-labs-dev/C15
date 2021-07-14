package com.nonlinearlabs.client.world.maps.presets.bank;

import java.util.ArrayList;
import java.util.HashMap;

import com.google.gwt.canvas.dom.client.Context2d;
import com.google.gwt.xml.client.Node;
import com.google.gwt.xml.client.NodeList;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.Renameable;
import com.nonlinearlabs.client.ServerProxy;
import com.nonlinearlabs.client.Tracer;
import com.nonlinearlabs.client.dataModel.setup.SetupModel;
import com.nonlinearlabs.client.dataModel.setup.SetupModel.BooleanValues;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.IBank;
import com.nonlinearlabs.client.world.IPreset;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.maps.Label;
import com.nonlinearlabs.client.world.maps.LayoutResizingVertical;
import com.nonlinearlabs.client.world.maps.MapsControl;
import com.nonlinearlabs.client.world.maps.NonDimension;
import com.nonlinearlabs.client.world.maps.NonPosition;
import com.nonlinearlabs.client.world.maps.NonRect;
import com.nonlinearlabs.client.world.maps.presets.MultiplePresetSelection;
import com.nonlinearlabs.client.world.maps.presets.PresetManager;
import com.nonlinearlabs.client.world.maps.presets.bank.Tape.Orientation;
import com.nonlinearlabs.client.world.maps.presets.bank.preset.Preset;
import com.nonlinearlabs.client.world.overlay.DragProxy;
import com.nonlinearlabs.client.world.overlay.belt.EditBufferDraggingButton;

public class Bank extends LayoutResizingVertical implements Renameable, IBank {

	private Position dragPosition = null;
	private DropAction currentDropAction;
	private IPreset currentDropPreset;
	private String uuid = "";
	final Header header = new Header(this);
	private boolean collapsed = false;
	private Label emptyLabel = null;
	private PrevNextButtons prevNext;
	private int orderNumber = 0;
	private HashMap<String, String> attributes = new HashMap<String, String>();
	private String dateOfLastChange = "---";

	private Bank masterLeft = null;
	private Bank masterTop = null;
	private Bank slaveRight = null;
	private Bank slaveBottom = null;

	private boolean mouseCaptured = false;
	private String masterUUID = "";
	private String attatchDirection = "";
	private String m_saveState = "";

	public boolean isInCluster() {
		return masterLeft != null || masterTop != null || slaveBottom != null || slaveRight != null;
	}

	private enum DropPosition {
		ABOVE, COVERING, BELOW
	}

	public enum DropAction {
		NONE, COPY_PRESET, MOVE_PRESET, INSERT_EDITBUFFER, INSERT_BANK, DROP_PRESETS
	}

	private Tape tapes[] = new Tape[4];
	private PresetList presetList = null;

	public Bank(PresetManager parent, String uuid) {
		super(parent);
		this.uuid = uuid;
		addChild(header);
		addChild(presetList = new PresetList(this));
		prevNext = addChild(new PrevNextButtons(this));

		tapes[Tape.Orientation.North.ordinal()] = addChild(new Tape(this, Tape.Orientation.North));
		tapes[Tape.Orientation.South.ordinal()] = addChild(new Tape(this, Tape.Orientation.South));
		tapes[Tape.Orientation.East.ordinal()] = addChild(new Tape(this, Tape.Orientation.East));
		tapes[Tape.Orientation.West.ordinal()] = addChild(new Tape(this, Tape.Orientation.West));
	}

	public boolean isDockedInDirection(Orientation orientation) {
		switch (orientation) {
			case North:
				return getMasterTop() != null;
			case South:
				return getBottomSlave() != null;
			case East:
				return getRightSlave() != null;
			case West:
				return getMasterLeft() != null;
		}
		return false;
	}

	@Override
	public boolean skipChildOnLayout(MapsControl c) {
		return c instanceof Tape;
	}

	public Tape getTape(Tape.Orientation o) {
		return tapes[o.ordinal()];
	}

	@Override
	public void doFirstLayoutPass(double levelOfDetail) {
		super.doFirstLayoutPass(levelOfDetail);
		NonDimension oldDim = getNonPosition().getDimension();

		double tapeSize = getAttachArea();

		for (MapsControl c : getChildren()) {
			c.getNonPosition().moveBy(tapeSize, tapeSize);
		}

		for (Tape t : tapes) {
			t.layout(oldDim);
		}

		setNonSize(oldDim.getWidth() + tapeSize * 2, oldDim.getHeight() + tapeSize * 2);
	}

	@Override
	public void draw(Context2d ctx, int invalidationMask) {

		if (isDraggingControl() && !isVisibilityForced())
			return;

		super.draw(ctx, invalidationMask);

		Rect r = getPixRect().copy();
		double reduce = toXPixels(getAttachArea());
		r = r.getReducedBy(2 * reduce);
		r.drawRoundedRect(ctx, Rect.ROUNDING_TOP, toXPixels(6), toXPixels(3), null, getColorBankSelect());

		for (Control c : presetList.getChildren()) {
			c.drawPost(ctx, invalidationMask);
		}

		drawDropIndicator(ctx);
	}

	protected void drawDropIndicator(Context2d ctx) {
		if (dragPosition != null) {
			for (Control c : presetList.getChildren()) {
				if (c instanceof IPreset) {
					Rect presetRect = c.getPixRect();

					if (presetRect.contains(dragPosition)) {
						drawDropIndicator(ctx, presetRect);
						return;
					} else if (presetRect.contains(
							new Position(dragPosition.getX(), dragPosition.getY() + toYPixels(getPadding())))) {
						drawDropIndicator(ctx, presetRect);
						return;
					} else if (presetRect.contains(
							new Position(dragPosition.getX(), dragPosition.getY() - toYPixels(getPadding())))) {
						drawDropIndicator(ctx, presetRect);
						return;
					}
				}
			}
		}
	}

	public double getAttachArea() {
		return 40;
	}

	public double getVisibleAttachArea() {
		return 20;
	}

	private double getSlaveDistance() {
		return 30;
	}

	private void drawDropIndicator(Context2d ctx, Rect presetRect) {
		ctx.setFillStyle("rgba(255, 0, 0, 0.5)");

		switch (getDropPosition(presetRect, dragPosition)) {
			case ABOVE:
				drawDropIndicator(ctx, presetRect, -0.1, 0.2);
				break;

			case BELOW:
				drawDropIndicator(ctx, presetRect, 0.9, 0.2);
				break;

			case COVERING:
				drawDropIndicator(ctx, presetRect, 0.1, 0.8);
				break;
		}
	}

	private void drawDropIndicator(Context2d ctx, Rect rect, double yOffset, double heightFactor) {
		ctx.fillRect(rect.getLeft(), rect.getTop() + rect.getHeight() * yOffset, rect.getWidth(),
				rect.getHeight() * heightFactor);
	}

	@Override
	public void dragLeave() {
		dragPosition = null;
		currentDropAction = DropAction.NONE;
		invalidate(INVALIDATION_FLAG_UI_CHANGED);
	}

	@Override
	public Control drag(Position pos, DragProxy dragProxy) {
		if (!isDraggingControl()) {
			for (Control c : presetList.getChildren()) {
				if (c instanceof IPreset) {
					Rect presetRect = c.getPixRect();

					if (presetRect.contains(pos)
							|| presetRect.contains(new Position(pos.getX(), pos.getY() - toYPixels(getPadding())))
							|| presetRect.contains(new Position(pos.getX(), pos.getY() + toYPixels(getPadding())))) {

						currentDropAction = getDropAction(pos, dragProxy);
						currentDropPreset = (IPreset) c;

						if (currentDropAction != DropAction.NONE) {
							dragPosition = pos;
							invalidate(INVALIDATION_FLAG_UI_CHANGED);
							return this;
						}
					}
				}
			}
		}
		currentDropAction = DropAction.NONE;
		return super.drag(pos, dragProxy);
	}

	@Override
	public Control drop(Position pos, DragProxy dragProxy) {
		if (dragPosition != null && currentDropPreset != null) {
			Control c = (Control) currentDropPreset;
			DropPosition dropPosition = getDropPosition(c.getPixRect(), dragPosition);
			doDropAction(currentDropAction, dropPosition, currentDropPreset, dragProxy);
			Tracer.log(currentDropAction.toString() + " " + dropPosition.toString());
			dragPosition = null;
			currentDropAction = DropAction.NONE;
			invalidate(INVALIDATION_FLAG_UI_CHANGED);
			return this;
		}

		return null;
	}

	private void doDropAction(DropAction action, DropPosition pos, IPreset preset, DragProxy dragProxy) {
		switch (action) {
			case COPY_PRESET:
				doCopyPreset(pos, preset, dragProxy);
				break;

			case INSERT_EDITBUFFER:
				doInsertEditbuffer(pos, preset, dragProxy);
				break;

			case MOVE_PRESET:
				doMovePreset(pos, preset, dragProxy);
				break;

			case INSERT_BANK:
				doInsertBank(pos, preset, dragProxy);
				break;

			case DROP_PRESETS:
				doDropPresets(pos, preset);

			case NONE:
			default:
				break;
		}
	}

	private void doDropPresets(DropPosition pos, IPreset preset) {
		PresetManager pm = NonMaps.get().getNonLinearWorld().getPresetManager();
		MultiplePresetSelection sel = pm.getMultiSelection();
		String csv = sel.getCSV();

		switch (pos) {
			case ABOVE:
				getNonMaps().getServerProxy().dropPresetsAbove(csv, preset);
				break;

			case BELOW:
				getNonMaps().getServerProxy().dropPresetsBelow(csv, preset);
				break;

			case COVERING:
				getNonMaps().getServerProxy().dropPresetsTo(csv, preset);
				break;
		}
	}

	private void doMovePreset(DropPosition pos, IPreset preset, DragProxy dragProxy) {
		IPreset p = (IPreset) dragProxy.getOrigin();

		switch (pos) {
			case ABOVE:
				getNonMaps().getServerProxy().movePresetAbove(p, preset);
				break;

			case BELOW:
				getNonMaps().getServerProxy().movePresetBelow(p, preset);
				break;

			case COVERING:
				getNonMaps().getServerProxy().movePresetTo(p, preset);
				break;
		}
	}

	private void doInsertEditbuffer(DropPosition pos, IPreset preset, DragProxy dragProxy) {
		switch (pos) {
			case ABOVE:
				getNonMaps().getServerProxy().insertEditBufferAbove(preset);
				break;

			case BELOW:
				getNonMaps().getServerProxy().insertEditBufferBelow(preset);
				break;

			case COVERING:
				getNonMaps().getServerProxy().overwritePresetWithEditBuffer(preset);
				break;
		}
	}

	private void doInsertBank(DropPosition pos, IPreset preset, DragProxy dragProxy) {
		IBank bank = (IBank) dragProxy.getOrigin();

		Bank bBank = (Bank) bank;
		if (bBank.hasSlaves())
			return;

		switch (pos) {
			case ABOVE:
				getNonMaps().getServerProxy().insertBankAbove(bank, preset);
				break;

			case BELOW:
				getNonMaps().getServerProxy().insertBankBelow(bank, preset);
				break;

			case COVERING:
				getNonMaps().getServerProxy().overwritePresetWithBank(bank, preset);
				break;
		}
	}

	private void doCopyPreset(DropPosition pos, IPreset preset, DragProxy dragProxy) {
		IPreset p = (IPreset) dragProxy.getOrigin();

		switch (pos) {
			case ABOVE:
				getNonMaps().getServerProxy().insertPresetCopyAbove(p, preset);
				break;

			case BELOW:
				getNonMaps().getServerProxy().insertPresetCopyBelow(p, preset);
				break;

			case COVERING:
				getNonMaps().getServerProxy().overwritePresetWith(p, preset);
				break;
		}
	}

	private DropPosition getDropPosition(Rect rect, Position pt) {

		double diffFromTop = pt.getY() - rect.getTop();
		if (diffFromTop < rect.getHeight() * 0.25) {
			return DropPosition.ABOVE;
		} else if (diffFromTop > rect.getHeight() * 0.75) {
			return DropPosition.BELOW;
		} else {
			return DropPosition.COVERING;
		}
	}

	@Override
	public <T extends MapsControl> T addChild(T child) {
		T ret = super.addChild(child);

		if (prevNext != null) {
			removeChild(prevNext);
			super.addChild(prevNext);
		}

		return ret;
	}

	@Override
	public PresetManager getParent() {
		return (PresetManager) super.getParent();
	}

	@Override
	public Control click(Position eventPoint) {
		for (Tape t : tapes)
			if (t.getPixRect().contains(eventPoint))
				return null;

		return this;
	}

	public void setCollapsed(boolean min) {
		if (collapsed != min) {
			collapsed = min;
			NonMaps.get().getServerProxy().setBankCollapsed(this, Boolean.toString(collapsed));
			requestLayout();
		}
	}

	public void toggleMinMax() {
		setCollapsed(!collapsed);
	}

	public void onMouseLost() {
		mouseCaptured = false;
	}

	public void onMouseCaptured() {
		mouseCaptured = true;
	}

	public boolean isMouseCaptured() {
		return mouseCaptured;
	}

	public RGB getColorBankInnerBorder() {
		return new RGB(77, 77, 77);
	}

	@Override
	public double getXMargin() {
		return 2;
	}

	@Override
	public double getYMargin() {
		return 2;
	}

	public RGB getColorBankSelect() {
		if (NonMaps.get().getNonLinearWorld().getViewport().getOverlay().getDragProxyFor(this) != null)
			return new RGB(98, 113, 183);
		else if (isSelected())
			return new RGB(173, 181, 217);
		else
			return new RGB(150, 150, 150);
	}

	public String getUUID() {
		return uuid;
	}

	@Override
	public RGB getColorFont() {
		return new RGB(250, 250, 250);
	}

	@Override
	public RGB getColorObjectContour() {
		if (isSelected())
			return new RGB(255, 255, 255);

		return new RGB(125, 125, 125);
	}

	public boolean isSelected() {
		if (getParent().hasCustomPresetSelection())
			return getParent().getCustomPresetSelection().getSelectedBank() == this;

		return uuid.equals(getParent().getSelectedBank());
	}

	public String getCurrentName() {
		return header.getBankName();
	}

	public String getTitleName() {
		return getCurrentName();
	}

	public void setName(String newName) {
		getNonMaps().getServerProxy().renameBank(getUUID(), newName);
	}

	public String getEntityName() {
		return "Bank";
	}

	public void selectBank(boolean userInteraction) {
		getParent().selectBank(getUUID(), userInteraction);
		invalidate(INVALIDATION_FLAG_UI_CHANGED);
	}

	public boolean isCollapsed() {
		return collapsed;
	}

	void showEmptyLabel(boolean bankEmpty) {
		if (bankEmpty && emptyLabel == null) {
			emptyLabel = addChild(new EmptyLabel(this, this, "- empty -"));
		} else if (!bankEmpty && emptyLabel != null) {
			removeChild(emptyLabel);
			emptyLabel = null;
		}
	}

	public void setOrderNumber(int i) {
		orderNumber = i;
	}

	public int getOrderNumber() {
		return orderNumber;
	}

	protected DropAction getDropAction(Position pos, DragProxy draggedElement) {

		if (SetupModel.get().localSettings.presetDragDrop.getValue() == BooleanValues.on) {

			Control origin = draggedElement.getOrigin();

			if (origin instanceof IPreset) {
				IPreset p = (IPreset) origin;

				PresetManager pm = getParent();

				Preset targetPreset = findPresetAt(pos);

				if (targetPreset == null) {
					targetPreset = findPresetAt(new Position(pos.getX(), pos.getY() + toYPixels(getPadding())));

					if (targetPreset == null) {
						targetPreset = findPresetAt(new Position(pos.getX(), pos.getY() - toYPixels(getPadding())));
					}
				}

				if (pm.hasMultiplePresetSelection() && pm.getMultiSelection().getNumSelectedPresets() > 1) {
					if (!pm.getMultiSelection().getSelectedPresets().contains(targetPreset.getUUID()))
						return DropAction.DROP_PRESETS;
					else
						return DropAction.NONE;
				}

				if (targetPreset != null && targetPreset.getUUID().equals(p.getUUID())) {
					return DropAction.NONE;
				} else if (targetPreset == null) {
					return DropAction.COPY_PRESET;
				}

				if (presetList.findPreset(p.getUUID()) != null) {
					return DropAction.MOVE_PRESET;
				}

				return DropAction.COPY_PRESET;
			}

			if (origin instanceof EditBufferDraggingButton)
				return DropAction.INSERT_EDITBUFFER;

			if (origin instanceof IBank) {
				if (!origin.equals(this)) {
					Bank bBank = (Bank) origin;
					if (!bBank.hasSlaves())
						return DropAction.INSERT_BANK;
				}
			}
		}

		return DropAction.NONE;
	}

	private Preset findPresetAt(Position pos) {
		for (Control c : presetList.getChildren())
			if (c instanceof Preset)
				if (c.getPixRect().contains(pos))
					return (Preset) c;

		return null;
	}

	public Preset getPreset(int idx) {
		for (Control c : presetList.getChildren()) {
			if (c instanceof Preset) {
				if (idx <= 0) {
					return (Preset) c;
				}
				idx--;
			}
		}

		return null;
	}

	public Preset getLast() {
		Control last = null;
		for (Control c : presetList.getChildren())
			if (c instanceof Preset)
				last = c;

		return (Preset) last;
	}

	@Override
	public Control doubleClick(Position pos) {
		return this;
	}

	public boolean isEmpty() {
		return getPresetList().getChildren().isEmpty();
	}

	void updateAttributes(Node node) {
		attributes.clear();

		NodeList children = node.getChildNodes();

		for (int i = 0; i < children.getLength(); i++) {
			Node n = children.item(i);
			String nodesName = n.getNodeName();

			if (nodesName.equals("attribute")) {
				updateAttribute(n);
			}
		}
	}

	private void updateAttribute(Node n) {
		String key = n.getAttributes().getNamedItem("key").getNodeValue();
		String value = ServerProxy.getText(n);
		attributes.put(key, value);
	}

	public String getAttribute(String key) {
		String ret = attributes.get(key);
		if (ret != null)
			return ret;
		return "";
	}

	public void setDateOfLastChange(String d) {
		dateOfLastChange = d;
	}

	public String getDateOfLastChange() {
		return dateOfLastChange;
	}

	public void setAttribute(String key, String value) {
		attributes.put(key, value);
		NonMaps.get().getServerProxy().setBankAttribute(this, key, value);
		requestLayout();
	}

	public void undockBank() {
		NonMaps.theMaps.getServerProxy().undockBank(this);
	}

	public Bank getBottomSlave() {
		return slaveBottom;
	}

	public Bank getRightSlave() {
		return slaveRight;
	}

	public void installRelationshipMasterSlave(Bank master) {
		String direction = getAttatchDirection();

		if (direction.equals("top") || direction.equals("bottom")) {
			masterTop = master;
			masterLeft = null;
			masterTop.addSlave(this, "bottom");
		} else if (direction.equals("left") || direction.equals("right")) {
			masterTop = null;
			masterLeft = master;
			masterLeft.addSlave(this, "right");
		} else {
			return;
		}
	}

	public void addSlave(Bank slave, String direction) {
		if (direction.equals("bottom") || direction.equals("top")) {
			if (slaveBottom == null)
				slaveBottom = slave;
		} else if (direction.equals("right") || direction.equals("left")) {
			if (slaveRight == null)
				slaveRight = slave;
		}
	}

	public Bank getMaster() {
		if (masterLeft != null)
			return masterLeft;
		else if (masterTop != null)
			return masterTop;
		else
			return null;
	}

	public void removeMaster() {
		masterLeft = null;
		masterTop = null;
	}

	public void removeSlave(Bank slave) {
		if (slaveBottom == slave)
			slaveBottom = null;
		else if (slaveRight == slave)
			slaveRight = null;
	}

	public Orientation getDirectionOfMaster() {
		if (masterLeft != null)
			return Orientation.West;
		else if (masterTop != null)
			return Orientation.North;
		return null;
	}

	public void removeAllSlaves() {
		slaveBottom = null;
		slaveRight = null;
	}

	public void setAttatchedTo(String direction) {
		if (!this.attatchDirection.equals(direction)) {
			this.attatchDirection = direction;
			requestLayout();
		}
	}

	public void setMasterUUID(String masterUUID) {
		if (!this.masterUUID.equals(masterUUID)) {
			this.masterUUID = masterUUID;
			requestLayout();
		}
	}

	public String getMasterUUID() {
		return this.masterUUID;
	}

	public String getAttatchDirection() {
		return this.attatchDirection;
	}

	public void collectCluster(ArrayList<Bank> ret) {
		ret.add(this);

		if (slaveBottom != null)
			slaveBottom.collectCluster(ret);

		if (slaveRight != null)
			slaveRight.collectCluster(ret);
	}

	public void layoutSlaves() {
		if (slaveBottom != null) {
			NonPosition posYFin = new NonPosition(getNonPosition().getLeft(), getNonPosition().getBottom()
					- this.getAttachArea() - slaveBottom.getAttachArea() + getSlaveDistance());
			posYFin.snapTo(PresetManager.getSnapGridResolution());
			slaveBottom.moveTo(posYFin);
			slaveBottom.layoutSlaves();
		}

		if (slaveRight != null) {
			NonPosition posXFin = new NonPosition(getNonPosition().getRight() - this.getAttachArea()
					- slaveRight.getAttachArea() + getSlaveDistance(), getNonPosition().getTop());
			posXFin.snapTo(PresetManager.getSnapGridResolution());
			slaveRight.moveTo(posXFin);
			slaveRight.layoutSlaves();
		}
	}

	public Header getHeader() {
		return header;
	}

	public boolean hasSlaves() {
		return getBottomSlave() != null || getRightSlave() != null;
	}

	public void setImportExportState(String state) {
		m_saveState = state;
	}

	public String getImportExportState() {
		return m_saveState;
	}

	public boolean isTapeActive(Orientation orientation) {
		boolean hasMaster = masterLeft != null || masterTop != null;

		switch (orientation) {
			case North:
				return !hasMaster;

			case South:
				return slaveBottom == null;

			case East: {
				Bank walker = this;
				while (walker != null) {
					if (walker.slaveRight != null)
						return false;
					walker = walker.masterTop;
				}
				return true;
			}

			case West:
				return !hasMaster;
		}
		return false;
	}

	public Tape[] getTapes() {
		return tapes;
	}

	public Bank getMasterTop() {
		return masterTop;
	}

	public Bank getMasterLeft() {
		return masterLeft;
	}

	@Override
	public void beingDragged(double xDiff, double yDiff) {
		getParent().resetAttachingTapes();
		super.beingDragged(xDiff, yDiff);
	}

	public boolean isClusteredWith(Bank parent) {
		Bank master = getClusterMaster();
		return master == parent.getClusterMaster();
	}

	public Bank getClusterMaster() {
		Bank master = this;
		while (true) {
			if (master.masterLeft != null)
				master = master.masterLeft;
			else if (master.masterTop != null)
				master = master.masterTop;
			else
				return master;
		}
	}

	public PresetList getPresetList() {
		return presetList;
	}

	@Override
	protected NonDimension layoutChildren(double levelOfDetail) {
		NonDimension dim = super.layoutChildren(levelOfDetail);
		NonRect pos = header.getNonPosition();
		double border = 3;
		pos.moveBy(-border, -border);
		pos.getDimension().setWidth(pos.getDimension().getWidth() + 2 * border);
		pos.getDimension().setHeight(pos.getDimension().getHeight() + border);
		return dim;
	}

	public Tape getAttachedTapeInDirection(Orientation orientation) {
		Bank bottomSlave = getBottomSlave();
		Bank rightSlave = getRightSlave();
		Bank masterTop = getMasterTop();
		Bank masterLeft = getMasterLeft();

		switch (orientation) {
			case East:
				if (rightSlave != null)
					return rightSlave.getTape(Orientation.West);
				break;
			case North:
				if (masterTop != null)
					return masterTop.getTape(Orientation.South);
				break;
			case South:
				if (bottomSlave != null)
					return bottomSlave.getTape(Orientation.North);
				break;
			case West:
				if (masterLeft != null)
					return masterLeft.getTape(Orientation.East);
				break;
			default:
				break;

		}

		return null;
	}

	public boolean isMidiBank() {
		return NonMaps.get().getNonLinearWorld().getPresetManager().getMidiBank() == this;
	}
}
