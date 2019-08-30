#include "LPCProxy.h"
#include "Application.h"
#include "presets/PresetManager.h"
#include "presets/EditBuffer.h"
#include "parameters/Parameter.h"
#include "parameters/ModulateableParameter.h"
#include "parameters/PhysicalControlParameter.h"
#include <parameters/RibbonParameter.h>
#include "MessageParser.h"
#include "ParameterMessageComposer.h"
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

LPCProxy::LPCProxy()
    : m_lastTouchedRibbon(HardwareSourcesGroup::getUpperRibbonParameterID())
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

connection LPCProxy::onRibbonTouched(slot<void, int> s)
{
  return m_signalRibbonTouched.connectAndInit(s, m_lastTouchedRibbon);
}

connection LPCProxy::onLPCSoftwareVersionChanged(slot<void, int> s)
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
  if(msg.type == MessageParser::PARAM)
  {
    onParamMessageReceived(msg);
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

void LPCProxy::onParamMessageReceived(const MessageParser::NLMessage &msg)
{
  DebugLevel::info("it is a param message");

  uint16_t id = msg.params[0];
  notifyRibbonTouch(id);

  gint16 value = separateSignedBitToComplementary(msg.params[1]);
  auto param = Application::get().getPresetManager()->getEditBuffer()->findParameterByID(id);

  if(auto p = dynamic_cast<PhysicalControlParameter *>(param))
  {
    DebugLevel::info("param:", p->getMiniParameterEditorName(), ": ", value);
    applyParamMessageAbsolutely(p, value);
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
  if(ribbonsParameterID == HardwareSourcesGroup::getLowerRibbonParameterID()
     || ribbonsParameterID == HardwareSourcesGroup::getUpperRibbonParameterID())
  {
    m_lastTouchedRibbon = ribbonsParameterID;
    m_signalRibbonTouched.send(ribbonsParameterID);
  }
}

void LPCProxy::sendParameter(const Parameter *param)
{
  if(!m_suppressParamChanges)
  {
    tMessageComposerPtr cmp(new ParameterMessageComposer(param));
    queueToLPC(cmp);
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

void LPCProxy::traceBytes(const RefPtr<Bytes> bytes) const
{
  if(Application::get().getSettings()->getSetting<DebugLevel>()->getLevel() == DebugLevels::DEBUG_LEVEL_GASSY)
  {
    gsize numBytes = 0;
    uint8_t *data = (uint8_t *) bytes->get_data(numBytes);

    char txt[numBytes * 4];
    char *ptr = txt;

    for(size_t i = 0; i < numBytes; i++)
      ptr += sprintf(ptr, "%02x ", data[i]);

    *ptr = '\0';
    DebugLevel::gassy((const char *) txt);
  }
}

void LPCProxy::sendEditBuffer()
{
  DebugLevel::info("send preset to LPC");

  auto editBuffer = Application::get().getPresetManager()->getEditBuffer();

  tMessageComposerPtr cmp(new EditBufferMessageComposer());

  auto sorted = editBuffer->getParametersSortedById();

  for(auto &it : sorted)
    it.second->writeToLPC(*cmp);

  queueToLPC(cmp);
  Application::get().getSettings()->sendToLPC();

  for(auto &it : sorted)
    it.second->onPresetSentToLpc();
}

void LPCProxy::toggleSuppressParameterChanges(UNDO::Transaction *transaction)
{
  transaction->addSimpleCommand([=](UNDO::Command::State) mutable {
    m_suppressParamChanges = !m_suppressParamChanges;

    if(!m_suppressParamChanges)
    {
      sendEditBuffer();
    }
  });
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
