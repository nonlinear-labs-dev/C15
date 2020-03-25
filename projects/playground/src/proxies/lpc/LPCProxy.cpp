#include "LPCProxy.h"
#include "Application.h"
#include "presets/PresetManager.h"
#include "presets/EditBuffer.h"
#include "parameters/Parameter.h"
#include "parameters/ModulateableParameter.h"
#include "parameters/PhysicalControlParameter.h"
#include <parameters/RibbonParameter.h>
#include "MessageParser.h"
#include "EditBufferMessageComposer.h"
#include "libundo/undo/Scope.h"
#include "http/UndoScope.h"
#include "proxies/hwui/Layout.h"
#include "device-settings/SendPresetAsLPCWriteFallback.h"
#include "device-settings/Settings.h"
#include "proxies/hwui/HWUI.h"
#include "groups/HardwareSourcesGroup.h"
#include "device-settings/DebugLevel.h"
#include "device-settings/VelocityCurve.h"
#include <device-settings/ParameterEditModeRibbonBehaviour.h>
#include <memory.h>
#include <nltools/messaging/Message.h>
#include <proxies/audio-engine/AudioEngineProxy.h>

LPCProxy::LPCProxy()
    : m_lastTouchedRibbon(HardwareSourcesGroup::getUpperRibbonParameterID().getNumber())
    , m_throttledRelativeParameterChange(std::chrono::milliseconds(1))
    , m_throttledAbsoluteParameterChange(std::chrono::milliseconds(1))
{
  m_msgParser.reset(new MessageParser());

  nltools::msg::onConnectionEstablished(nltools::msg::EndPoint::Lpc, sigc::mem_fun(this, &LPCProxy::onLPCConnected));

  nltools::msg::receive<nltools::msg::LPCMessage>(nltools::msg::EndPoint::Playground,
                                                  sigc::mem_fun(this, &LPCProxy::onLPCMessage));
}

LPCProxy::~LPCProxy()
{
  DebugLevel::warning(__PRETTY_FUNCTION__, __LINE__);
}

void LPCProxy::onLPCMessage(const nltools::msg::LPCMessage &msg)
{
  gsize numBytes = 0;
  const uint8_t *buffer = (const uint8_t *) (msg.message->get_data(numBytes));

  if(numBytes > 0)
  {
    if(m_msgParser->parse(buffer, numBytes) == 0)
    {
      onMessageReceived(std::move(m_msgParser->getMessage()));
      m_msgParser.reset(new MessageParser());
    }
  }
}

sigc::connection LPCProxy::onRibbonTouched(sigc::slot<void, int> s)
{
  return m_signalRibbonTouched.connectAndInit(s, m_lastTouchedRibbon);
}

sigc::connection LPCProxy::onLPCSoftwareVersionChanged(sigc::slot<void, int> s)
{
  return m_signalLPCSoftwareVersionChanged.connectAndInit(s, m_lpcSoftwareVersion);
}

int LPCProxy::getLastTouchedRibbonParameterID() const
{
  return m_lastTouchedRibbon;
}

gint16 LPCProxy::separateSignedBitToComplementary(uint16_t v) const
{
  gint16 sign = (v & 0x8000) ? -1 : 1;
  gint16 value = (v & 0x7FFF);
  return sign * value;
}

void LPCProxy::onMessageReceived(const MessageParser::NLMessage &msg)
{
  if(msg.type == MessageParser::HARDWARE_SOURCE)
  {
    onHardwareSourceReceived(msg);
  }
  else if(msg.type == MessageParser::EDIT_CONTROL)
  {
    onEditControlMessageReceived(msg);
  }
  else if(msg.type == MessageParser::ASSERTION)
  {
    onAssertionMessageReceived(msg);
  }
  else if(msg.type == MessageParser::NOTIFICATION)
  {
    onNotificationMessageReceived(msg);
  }
  else if(msg.type == MessageParser::HEARTBEAT)
  {
    onHeartbeatReceived(msg);
  }
}

void LPCProxy::onHeartbeatReceived(const MessageParser::NLMessage &msg)
{
  uint64_t heartbeat = *(reinterpret_cast<const uint64_t *>(msg.params.data()));

  DebugLevel::info("LPC Heartbeat", heartbeat);

  if(heartbeat < m_lastReceivedHeartbeat)
  {
    DebugLevel::warning("LPCProxy had to re-send the edit buffer, as the heartbeat stumbled from",
                        m_lastReceivedHeartbeat, "to", heartbeat);

    Application::get().getAudioEngineProxy()->sendEditBuffer();
  }

  m_lastReceivedHeartbeat = heartbeat;
}

void LPCProxy::onAssertionMessageReceived(const MessageParser::NLMessage &msg)
{
  auto numWords = msg.length;
  auto numBytes = numWords * 2;
  const char *bytes = (const char *) msg.params.data();
  char txt[numBytes + 2];
  strncpy(txt, bytes, numBytes);
  txt[numBytes] = '\0';
  DebugLevel::error("!!! Assertion from LPC in", (const char *) txt);
}

void LPCProxy::onNotificationMessageReceived(const MessageParser::NLMessage &msg)
{
  uint16_t id = msg.params[0];
  uint16_t value = msg.params[1];

  if(id == MessageParser::SOFTWARE_VERSION)
  {
    if(m_lpcSoftwareVersion != value)
    {
      m_lpcSoftwareVersion = value;
      m_signalLPCSoftwareVersionChanged.send(m_lpcSoftwareVersion);
    }
  }
}

void LPCProxy::onLPCConnected()
{
  requestLPCSoftwareVersion();
}

Parameter *LPCProxy::findPhysicalControlParameterFromLPCHWSourceID(uint16_t id) const
{
  auto paramId = [](uint16_t id) {
    switch(id)
    {
      case 0:
        return HardwareSourcesGroup::getPedal1ParameterID();
      case 1:
        return HardwareSourcesGroup::getPedal2ParameterID();
      case 2:
        return HardwareSourcesGroup::getPedal3ParameterID();
      case 3:
        return HardwareSourcesGroup::getPedal4ParameterID();
      case 4:
        return HardwareSourcesGroup::getPitchbendParameterID();
      case 5:
        return HardwareSourcesGroup::getAftertouchParameterID();
      case 6:
      case 284:
        return HardwareSourcesGroup::getUpperRibbonParameterID();
      case 7:
      case 289:
        return HardwareSourcesGroup::getLowerRibbonParameterID();
      default:
        return ParameterId::invalid();
    }
  }(id);

  return Application::get().getPresetManager()->getEditBuffer()->findParameterByID(paramId);
}

void LPCProxy::onHardwareSourceReceived(const MessageParser::NLMessage &msg)
{
  uint16_t id = msg.params[0];
  DebugLevel::info("received hw source message with hw source id:", id);

  gint16 value = separateSignedBitToComplementary(msg.params[1]);

  if(auto *param = dynamic_cast<PhysicalControlParameter *>(findPhysicalControlParameterFromLPCHWSourceID(id)))
  {
    notifyRibbonTouch(param->getID().getNumber());
    DebugLevel::info("physical control parameter:", param->getMiniParameterEditorName(), ": ", value);
    applyParamMessageAbsolutely(param, value);
  }
  else
  {
    DebugLevel::info("could not parse hw id", id, " to physical control parameter");
  }
}

void LPCProxy::applyParamMessageAbsolutely(PhysicalControlParameter *p, gint16 value)
{
  DebugLevel::info(G_STRLOC, value);

  if(p->isBiPolar())
  {
    DebugLevel::info(G_STRLOC);
    p->onChangeFromLpc((value - 8000.0) / 8000.0);
  }
  else
  {
    DebugLevel::info(G_STRLOC);
    p->onChangeFromLpc(value / 16000.0);
  }
}

void LPCProxy::onEditControlMessageReceived(const MessageParser::NLMessage &msg)
{
  DebugLevel::info("it is an edit control message");

  uint16_t id = msg.params[0];
  notifyRibbonTouch(id);

  gint16 value = separateSignedBitToComplementary(msg.params[1]);

  if(auto p = Application::get().getPresetManager()->getEditBuffer()->getSelected())
  {
    auto ribbonModeBehaviour = Application::get().getSettings()->getSetting<ParameterEditModeRibbonBehaviour>()->get();

    if(ribbonModeBehaviour == ParameterEditModeRibbonBehaviours::PARAMETER_EDIT_MODE_RIBBON_BEHAVIOUR_RELATIVE)
    {
      DebugLevel::info(G_STRLOC, value);
      onRelativeEditControlMessageReceived(p, value);
    }
    else
    {
      DebugLevel::info(G_STRLOC, value);
      onAbsoluteEditControlMessageReceived(p, value);
    }
  }
}

void LPCProxy::onRelativeEditControlMessageReceived(Parameter *p, gint16 value)
{
  m_throttledRelativeParameterAccumulator += value;

  m_throttledRelativeParameterChange.doTask([this, p]() {
    if(!m_relativeEditControlMessageChanger || !m_relativeEditControlMessageChanger->isManaging(p->getValue()))
      m_relativeEditControlMessageChanger = p->getValue().startUserEdit(Initiator::EXPLICIT_LPC);

    m_relativeEditControlMessageChanger->changeBy(m_throttledRelativeParameterAccumulator
                                                  / (p->isBiPolar() ? 8000.0 : 16000.0));
    m_throttledRelativeParameterAccumulator = 0;
  });
}

void LPCProxy::onAbsoluteEditControlMessageReceived(Parameter *p, gint16 value)
{
  m_throttledAbsoluteParameterValue = value;

  m_throttledAbsoluteParameterChange.doTask([this, p]() {
    auto scope
        = Application::get().getUndoScope()->startContinuousTransaction(p, "Set '%0'", p->getGroupAndParameterName());

    if(p->isBiPolar())
    {
      p->setCPFromHwui(scope->getTransaction(), (m_throttledAbsoluteParameterValue - 8000.0) / 8000.0);
      DebugLevel::info("set it (absolutely - bipolar) to", p->getControlPositionValue());
    }
    else
    {
      p->setCPFromHwui(scope->getTransaction(), (m_throttledAbsoluteParameterValue / 16000.0));
      DebugLevel::info("set it (absolutely - unipolar) to", p->getControlPositionValue());
    }
  });
}

void LPCProxy::notifyRibbonTouch(int ribbonsParameterID)
{
  if(ribbonsParameterID == HardwareSourcesGroup::getLowerRibbonParameterID().getNumber()
     || ribbonsParameterID == HardwareSourcesGroup::getUpperRibbonParameterID().getNumber())
  {
    m_lastTouchedRibbon = ribbonsParameterID;
    m_signalRibbonTouched.send(ribbonsParameterID);
  }
}

void LPCProxy::queueToLPC(tMessageComposerPtr cmp)
{
  auto flushed = cmp->flush();
  traceBytes(flushed);

  nltools::msg::LPCMessage msg;
  msg.message = flushed;

  nltools::msg::send(nltools::msg::EndPoint::Lpc, msg);
}

void LPCProxy::traceBytes(const Glib::RefPtr<Glib::Bytes> &bytes) const
{
  if(Application::get().getSettings()->getSetting<DebugLevel>()->get() == DebugLevels::DEBUG_LEVEL_GASSY)
  {
    gsize numBytes = 0;
    auto data = static_cast<const uint8_t *>(bytes->get_data(numBytes));

    char txt[numBytes * 4];
    char *ptr = txt;

    for(size_t i = 0; i < numBytes; i++)
      ptr += sprintf(ptr, "%02x ", data[i]);

    *ptr = '\0';
    DebugLevel::gassy((const char *) txt);
  }
}

void LPCProxy::sendSetting(uint16_t key, uint16_t value)
{
  tMessageComposerPtr cmp(new MessageComposer(MessageParser::SETTING));
  *cmp << key;
  *cmp << value;
  queueToLPC(cmp);

  DebugLevel::info("sending setting", key, "=", value);
}

void LPCProxy::sendSetting(uint16_t key, gint16 value)
{
  tMessageComposerPtr cmp(new MessageComposer(MessageParser::SETTING));
  *cmp << key;
  *cmp << value;
  queueToLPC(cmp);

  DebugLevel::info("sending setting", key, "=", value);
}

void LPCProxy::sendSetting(uint16_t key, bool value)
{
  sendSetting(key, (uint16_t)(value ? 1 : 0));
}

void LPCProxy::requestLPCSoftwareVersion()
{
  tMessageComposerPtr cmp(new MessageComposer(MessageParser::REQUEST));
  uint16_t v = MessageParser::SOFTWARE_VERSION;
  *cmp << v;
  queueToLPC(cmp);

  DebugLevel::info("sending request", MessageParser::SOFTWARE_VERSION);
}

int LPCProxy::getLPCSoftwareVersion() const
{
  return m_lpcSoftwareVersion;
}
