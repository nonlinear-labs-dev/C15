#pragma once

#include "playground.h"
#include "tools/RecursionGuard.h"
#include "libundo/undo/Scope.h"
#include "MessageParser.h"
#include "MessageComposer.h"
#include <parameters/value/RawValue.h>
#include <parameters/value/QuantizedValue.h>
#include <memory>
#include <nltools/threading/Throttler.h>

class Application;
class Parameter;
class ModulateableParameter;
class PhysicalControlParameter;
class Preset;
class MessageParser;

namespace nltools
{
  namespace msg
  {
    struct LPCMessage;
  }
}

enum LPCSettingIDs
{
  PLAY_MODE_UPPER_RIBBON_BEHAVIOUR = 0,      // ==> BIT 0 set if (returnMode == RETURN)
  PLAY_MODE_LOWER_RIBBON_BEHAVIOUR = 1,      // ... BIT 1 set if (touchBehaviour == RELATIVE)
  NOTE_SHIFT = 2,                            // ==> tTcdRange (-48, 48)
  BASE_UNIT_UI_MODE = 3,                     // ==> PLAY = 0, PARAMETER_EDIT = 1
  PARAMETER_EDIT_MODE_RIBBON_BEHAVIOUR = 4,  // ==> RELATIVE = 0, ABSOLUTE = 1
  PEDAL_1_MODE = 5,                          // ==> STAY = 0
  PEDAL_2_MODE = 6,                          // ... RETURN_TO_ZERO = 1
  PEDAL_3_MODE = 7,                          // ... RETURN_TO_CENTER = 2,
  PEDAL_4_MODE = 8,
  RIBBON_REL_FACTOR = 9,  // ==> tTcdRange(256, 2560)
  // obsolete LOWER_RIBBON_REL_FACTOR = 10, // ==> tTcdRange(256, 2560)
  VELOCITY_CURVE = 11,  // ==> VERY_SOFT = 0, SOFT = 1, NORMAL = 2, HARD = 3, VERY_HARD = 4

  TRANSITION_TIME = 12,  // ==> tTcdRange(0, 16000)

  PEDAL_1_TYPE = 26,  // ==> PotTipActive = 0
  PEDAL_2_TYPE = 27,  // ... PotRingActive = 1
  PEDAL_3_TYPE = 28,  // ... SwitchClosing = 2
  PEDAL_4_TYPE = 29,  // ... SwitchOpening = 3

  AFTERTOUCH_CURVE = 30,  // SOFT = 0, NORMAL = 1, HARD = 2
  BENDER_CURVE = 31,      // ZERO = 0, NARROW = 1, WIDE = 2

  PITCHBEND_ON_PRESSED_KEYS = 32,  // OFF = 0, ON = 1

  EDIT_SMOOTHING_TIME = 33,  // ==> tTcdRange(0, 16000)

  PRESET_GLITCH_SUPPRESSION = 34,  // OFF = 0, ON = 1

  BENDER_RAMP_BYPASS = 35  // OFF = 0, ON = 1
};

class LPCProxy
{
 public:
  LPCProxy();
  virtual ~LPCProxy();

  void sendParameter(const Parameter *param);
  void sendEditBuffer();
  void sendSetting(uint16_t key, gint16 value);
  void sendSetting(uint16_t key, uint16_t value);
  void sendSetting(uint16_t key, bool v);

  void toggleSuppressParameterChanges(UNDO::Transaction *transaction);

  connection onRibbonTouched(slot<void, int> s);
  connection onLPCSoftwareVersionChanged(slot<void, int> s);
  int getLastTouchedRibbonParameterID() const;

  void requestLPCSoftwareVersion();
  int getLPCSoftwareVersion() const;

 private:
  void onLPCMessage(const nltools::msg::LPCMessage &msg);
  void onMessageReceived(const MessageParser::NLMessage &msg);

  typedef std::shared_ptr<MessageComposer> tMessageComposerPtr;
  void queueToLPC(tMessageComposerPtr cmp);

  gint16 separateSignedBitToComplementary(uint16_t v) const;
  void traceBytes(const RefPtr<Bytes> bytes) const;

  void notifyRibbonTouch(int ribbonsParameterID);
  void onParamMessageReceived(const MessageParser::NLMessage &msg);
  void onEditControlMessageReceived(const MessageParser::NLMessage &msg);
  void onRelativeEditControlMessageReceived(Parameter *p, gint16 value);
  void onAbsoluteEditControlMessageReceived(Parameter *p, gint16 value);
  void applyParamMessageAbsolutely(PhysicalControlParameter *p, gint16 value);
  void onAssertionMessageReceived(const MessageParser::NLMessage &msg);
  void onNotificationMessageReceived(const MessageParser::NLMessage &msg);
  void deliverPendingLPCMessages();
  void onLPCConnected();

  bool m_suppressParamChanges = false;
  std::shared_ptr<MessageParser> m_msgParser;

  int m_lastTouchedRibbon;
  Signal<void, int> m_signalRibbonTouched;

  std::unique_ptr<QuantizedValue::IncrementalChanger> m_relativeEditControlMessageChanger;

  int m_lpcSoftwareVersion = 0;
  Signal<void, int> m_signalLPCSoftwareVersionChanged;

  Throttler m_throttledRelativeParameterChange;
  gint32 m_throttledRelativeParameterAccumulator = 0;

  Throttler m_throttledAbsoluteParameterChange;
  gint32 m_throttledAbsoluteParameterValue = 0;
};
