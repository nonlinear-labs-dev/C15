#pragma once

#include "playground.h"
#include "MessageParser.h"
#include "MessageComposer.h"
#include <parameters/value/QuantizedValue.h>
#include <memory>
#include <nltools/threading/Throttler.h>
#include <tools/Signal.h>
#include <device-settings/PedalType.h>

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
    struct PlaycontrollerMessage;
  }
}

enum PlaycontrollerSettingIDs
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

  AFTERTOUCH_CURVE = 30,  // SOFT = 0, NORMAL = 1, HARD = 2
  BENDER_CURVE = 31,      // ZERO = 0, NARROW = 1, WIDE = 2

  PITCHBEND_ON_PRESSED_KEYS = 32,  // OFF = 0, ON = 1

  EDIT_SMOOTHING_TIME = 33,  // ==> tTcdRange(0, 16000)

  PRESET_GLITCH_SUPPRESSION = 34,  // OFF = 0, ON = 1

  BENDER_RAMP_BYPASS = 35  // OFF = 0, ON = 1
};

class PlaycontrollerProxy
{
 public:
  PlaycontrollerProxy();
  virtual ~PlaycontrollerProxy();

  void sendSetting(uint16_t key, gint16 value);
  void sendSetting(uint16_t key, uint16_t value);

  void sendPedalSetting(uint16_t pedal, PedalTypes pedalType, bool reset);

  sigc::connection onRibbonTouched(sigc::slot<void, int> s);
  sigc::connection onPlaycontrollerSoftwareVersionChanged(const sigc::slot<void, int> &s);
  sigc::connection onLastKeyChanged(sigc::slot<void, int> s);
  int getLastTouchedRibbonParameterID() const;
  std::string getPlaycontrollerSoftwareVersion() const;
  Parameter *findPhysicalControlParameterFromPlaycontrollerHWSourceID(uint16_t id) const;
  void notifyRibbonTouch(int ribbonsParameterID);

 private:
  void onPlaycontrollerMessage(const nltools::msg::PlaycontrollerMessage &msg);
  void onMessageReceived(const MessageParser::NLMessage &msg);

  typedef std::shared_ptr<MessageComposer> tMessageComposerPtr;
  void queueToPlaycontroller(const tMessageComposerPtr &cmp);

  gint16 separateSignedBitToComplementary(uint16_t v) const;
  void traceBytes(const Glib::RefPtr<Glib::Bytes> &bytes) const;

  void onEditControlMessageReceived(const MessageParser::NLMessage &msg);
  void onRelativeEditControlMessageReceived(Parameter *p, gint16 value);
  void onAbsoluteEditControlMessageReceived(Parameter *p, gint16 value);
  void onAssertionMessageReceived(const MessageParser::NLMessage &msg);
  void onNotificationMessageReceived(const MessageParser::NLMessage &msg);
  void onPlaycontrollerConnected();
  void onHeartbeatReceived(const MessageParser::NLMessage &msg);
  void sendCalibrationData();

  std::shared_ptr<MessageParser> m_msgParser;

  int m_lastTouchedRibbon;
  Signal<void, int> m_signalRibbonTouched;
  Signal<void, int> m_signalPlaycontrollerSoftwareVersionChanged;
  Signal<void, int> m_lastKeyChanged;

  std::unique_ptr<QuantizedValue::IncrementalChanger> m_relativeEditControlMessageChanger;

  int m_playcontrollerSoftwareVersion = -1;

  Throttler m_throttledRelativeParameterChange;
  gint32 m_throttledRelativeParameterAccumulator = 0;

  Throttler m_throttledAbsoluteParameterChange;
  gint32 m_throttledAbsoluteParameterValue = 0;

  uint64_t m_lastReceivedHeartbeat = -1;

  void onHeartbeatStumbled();
  void requestPlaycontrollerSoftwareVersion();
  void notifyLastKey(gint16 key);
};
