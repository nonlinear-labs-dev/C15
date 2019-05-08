#pragma once

#include "playground.h"
#include "libundo/undo/Scope.h"
#include "scale-converters/ScaleConverter.h"
#include "http/UpdateDocumentContributor.h"
#include "value/QuantizedValue.h"
#include <proxies/hwui/HWUIEnums.h>
#include <tools/FlagOwner.h>

class Layout;
class DFBLayout;
class Application;
class ParameterLayout;
class ParameterInfoLayout;
class Writer;
class ParameterGroupSet;
class ParameterGroup;
class MessageComposer;
class FrameBuffer;

enum ParameterFlags
{
  Loaded = 1
};

class Parameter : public UpdateDocumentContributor,
                  public IntrusiveListItem<Parameter *>,
                  public FlagOwner<ParameterFlags, uint8_t>
{
 public:
  enum class Step
  {
    STEP_INC,
    STEP_DEC
  };

  enum class VisualizationStyle
  {
    Bar,
    Dot
  };

  Parameter(ParameterGroup *group, uint16_t id, const ScaleConverter *scaling, tControlPositionValue def,
            tControlPositionValue coarseDenominator, tControlPositionValue fineDenominator);
  virtual ~Parameter();

  tControlPositionValue expropriateSnapshotValue();

  const ParameterGroup *getParentGroup() const;
  ParameterGroup *getParentGroup();
  gint32 getID() const;

  bool isBiPolar() const;
  tControlPositionValue getDefaultValue() const;

  void setDefaultFromHwui();
  void setDefaultFromHwui(UNDO::Scope::tTransactionPtr transaction);
  void setDefaultFromWebUI(UNDO::Scope::tTransactionPtr transaction);

  virtual void setCPFromHwui(UNDO::Scope::tTransactionPtr transaction, const tControlPositionValue &cpValue);
  virtual void setCPFromWebUI(UNDO::Scope::tTransactionPtr transaction, const tControlPositionValue &cpValue);

  void stepCPFromHwui(UNDO::Scope::tTransactionPtr transaction, int incs, ButtonModifiers modifiers);
  void stepCPFromWebUI(UNDO::Scope::tTransactionPtr transaction, Step step, ButtonModifiers modifiers);
  void setIndirect(UNDO::Scope::tTransactionPtr transaction, const tControlPositionValue &value);

  virtual void loadFromPreset(UNDO::Scope::tTransactionPtr transaction, const tControlPositionValue &value);

  virtual void loadDefault(UNDO::Scope::tTransactionPtr transaction);
  virtual void reset(UNDO::Scope::tTransactionPtr transaction, Initiator initiator);
  virtual void copyFrom(UNDO::Scope::tTransactionPtr transaction, Parameter *other);

  virtual void undoableSetType(UNDO::Scope::tTransactionPtr transaction, PresetType oldType, PresetType desiredType);
  virtual void undoableRandomize(UNDO::Scope::tTransactionPtr transaction, Initiator initiator, double amount);

  void undoableLoadValue(UNDO::Scope::tTransactionPtr transaction, const Glib::ustring &value);
  void undoableSetDefaultValue(UNDO::Scope::tTransactionPtr transaction, Parameter *values);

  void undoableLock(UNDO::Scope::tTransactionPtr transaction);
  void undoableUnlock(UNDO::Scope::tTransactionPtr transaction);
  bool isLocked() const;

  virtual void exportReaktorParameter(stringstream &target) const;

  tTcdValue getTcdValue() const;
  tDisplayValue getDisplayValue() const;
  virtual tControlPositionValue getControlPositionValue() const;
  virtual Glib::ustring getDisplayString() const;

  QuantizedValue &getValue();
  const QuantizedValue &getValue() const;
  virtual size_t getHash() const;

  virtual void writeDocument(Writer &writer, tUpdateID knownRevision) const override;
  void writeDiff(Writer &writer, Parameter *other) const;
  virtual void writeToLPC(MessageComposer &cmp) const;
  virtual void onPresetSentToLpc() const;

  void invalidate();

  virtual Glib::ustring getLongName() const;
  virtual Glib::ustring getShortName() const;
  virtual Glib::ustring getMiniParameterEditorName() const;
  virtual Glib::ustring getGroupAndParameterName() const;
  virtual DFBLayout *createLayout(FocusAndMode focusAndMode) const;

  virtual void onUnselected();
  virtual void onSelected();

  virtual VisualizationStyle getVisualizationStyle() const;

  // CALLBACKS
  sigc::connection onParameterChanged(slot<void, const Parameter *> slot, bool doInitCall = true);

  void check();

 protected:
  virtual void sendToLpc() const;
  void setCpValue(UNDO::Scope::tTransactionPtr transaction, Initiator initiator, tControlPositionValue value,
                  bool doSendToLpc);
  virtual void writeDocProperties(Writer &writer, tUpdateID knownRevision) const;
  virtual void onValueChanged(Initiator initiator, tControlPositionValue oldValue, tControlPositionValue newValue);
  virtual bool shouldWriteDocProperties(tUpdateID knownRevision) const;
  virtual tControlPositionValue getNextStepValue(int incs, ButtonModifiers modifiers) const;
  virtual void writeDifferences(Writer &writer, Parameter *other) const;

 private:
  Signal<void, const Parameter *> m_signalParamChanged;

  uint16_t m_id;
  QuantizedValue m_value;
  sigc::connection m_valueChangedConnection;

  friend class QuantizedValue;
  friend class EditBufferSnapshotMaker;

  tControlPositionValue m_lastSnapshotedValue;
  bool m_isLocked = false;
};
