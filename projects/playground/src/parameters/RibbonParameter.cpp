#include "RibbonParameter.h"
#include "scale-converters/LinearBipolar100PercentScaleConverter.h"
#include "scale-converters/Linear100PercentScaleConverter.h"
#include "ModulationRoutingParameter.h"
#include <http/UpdateDocumentMaster.h>
#include <Application.h>
#include <proxies/lpc/LPCProxy.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/InfoLayout.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/ParameterInfoLayout.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/PlayControlParameterLayouts.h>
#include <groups/HardwareSourcesGroup.h>
#include <groups/MacroControlMappingGroup.h>
#include <xml/Writer.h>
#include <device-settings/DebugLevel.h>
#include <cmath>
#include <libundo/undo/Transaction.h>
#include <presets/ParameterDualGroupSet.h>
#include <presets/PresetParameter.h>
#include <presets/EditBuffer.h>
#include <nltools/messaging/Message.h>

void RibbonParameter::writeDocProperties(Writer &writer, UpdateDocumentContributor::tUpdateID knownRevision) const
{
  Parameter::writeDocProperties(writer, knownRevision);
  writer.writeTextElement("ribbon-touch-behaviour", to_string(m_touchBehaviour));
  writer.writeTextElement("ribbon-return-mode", to_string(m_returnMode));
}

void RibbonParameter::loadFromPreset(UNDO::Transaction *transaction, const tControlPositionValue &value)
{
  Parameter::loadFromPreset(transaction, value);
}

void RibbonParameter::undoableSetRibbonTouchBehaviour(UNDO::Transaction *transaction, RibbonTouchBehaviour mode)
{
  if(m_touchBehaviour != mode)
  {
    auto swapData = UNDO::createSwapData(mode);

    transaction->addSimpleCommand([=](UNDO::Command::State) mutable {
      swapData->swapWith(m_touchBehaviour);
      setupScalingAndDefaultValue();
    });
  }
  else
  {
    setupScalingAndDefaultValue();
  }
}

void RibbonParameter::setupScalingAndDefaultValue()
{
  getValue().setScaleConverter(createScaleConverter());
  getValue().setDefaultValue(getDefaultValueAccordingToMode());
  if(getReturnMode() != ReturnMode::None)
    getValue().setToDefault(Initiator::INDIRECT);

  bool routersAreBoolean = getReturnMode() == ReturnMode::None;

  if(auto groups = dynamic_cast<ParameterDualGroupSet *>(getParentGroup()->getParent()))
  {
    auto mappings
        = dynamic_cast<MacroControlMappingGroup *>(groups->getParameterGroupByID({ "MCM", VoiceGroup::Global }));

    for(auto router : mappings->getModulationRoutingParametersFor(this))
      router->getValue().setIsBoolean(routersAreBoolean);
  }

  ensureExclusiveRoutingIfNeeded();
  invalidate();
  sendModeToLpc();
  m_updateIdWhenModeChanged = getUpdateIDOfLastChange();
}

tControlPositionValue RibbonParameter::getDefaultValueAccordingToMode() const
{
  switch(getReturnMode())
  {
    case ReturnMode::None:
      return 0.5;

    case ReturnMode::Center:
    case ReturnMode::Zero:
      return 0.0;
  }

  return 0.0;
}

void RibbonParameter::undoableSetRibbonTouchBehaviour(UNDO::Transaction *transaction, const Glib::ustring &mode)
{
  if(mode == "absolute")
    undoableSetRibbonTouchBehaviour(transaction, RibbonTouchBehaviour::ABSOLUTE);
  else if(mode == "relative")
    undoableSetRibbonTouchBehaviour(transaction, RibbonTouchBehaviour::RELATIVE);
}

void RibbonParameter::undoableIncRibbonTouchBehaviour(UNDO::Transaction *transaction)
{
  auto e = static_cast<int>(m_touchBehaviour);
  e++;

  if(e >= static_cast<int>(RibbonTouchBehaviour::NUM_TOUCH_BEHAVIOURS))
    e = 0;

  undoableSetRibbonTouchBehaviour(transaction, static_cast<RibbonTouchBehaviour>(e));
}

RibbonTouchBehaviour RibbonParameter::getRibbonTouchBehaviour() const
{
  return m_touchBehaviour;
}

void RibbonParameter::undoableSetRibbonReturnMode(UNDO::Transaction *transaction, RibbonReturnMode mode)
{
  if(mode != RibbonReturnMode::STAY && mode != RibbonReturnMode::RETURN)
    mode = RibbonReturnMode::STAY;

  if(m_returnMode != mode)
  {
    auto swapData = UNDO::createSwapData(mode);

    transaction->addSimpleCommand([=](UNDO::Command::State) mutable {
      swapData->swapWith(m_returnMode);
      setupScalingAndDefaultValue();
      onChange();
    });
  }
  else
  {
    setupScalingAndDefaultValue();
    onChange();
  }
}

void RibbonParameter::undoableSetRibbonReturnMode(UNDO::Transaction *transaction, const Glib::ustring &mode)
{
  if(mode == "stay")
    undoableSetRibbonReturnMode(transaction, RibbonReturnMode::STAY);
  else if(mode == "return")
    undoableSetRibbonReturnMode(transaction, RibbonReturnMode::RETURN);
}

RibbonReturnMode RibbonParameter::getRibbonReturnMode() const
{
  return m_returnMode;
}

ReturnMode RibbonParameter::getReturnMode() const
{
  if(m_returnMode == RibbonReturnMode::RETURN)
    return ReturnMode::Center;

  return ReturnMode::None;
}

void RibbonParameter::ensureExclusiveRoutingIfNeeded()
{
  if(getRibbonReturnMode() == RibbonReturnMode::STAY)
  {
    if(auto groups = dynamic_cast<ParameterDualGroupSet *>(getParentGroup()->getParent()))
    {
      auto mappings
          = dynamic_cast<MacroControlMappingGroup *>(groups->getParameterGroupByID({ "MCM", VoiceGroup::Global }));
      auto routers = mappings->getModulationRoutingParametersFor(this);
      auto highest = *routers.begin();

      for(auto router : routers)
        if(highest != router)
          if(std::abs(router->getValue().getRawValue()) > std::abs(highest->getValue().getRawValue()))
            highest = router;

      for(auto router : routers)
        if(router != highest)
          router->onExclusiveRoutingLost();
    }
  }
}

bool RibbonParameter::shouldWriteDocProperties(UpdateDocumentContributor::tUpdateID knownRevision) const
{
  return Parameter::shouldWriteDocProperties(knownRevision) || knownRevision <= m_updateIdWhenModeChanged;
}

const ScaleConverter *RibbonParameter::createScaleConverter() const
{
  if(m_returnMode == RibbonReturnMode::RETURN)
    return ScaleConverter::get<LinearBipolar100PercentScaleConverter>();

  return ScaleConverter::get<Linear100PercentScaleConverter>();
}

void RibbonParameter::onPresetSentToLpc() const
{
  Parameter::onPresetSentToLpc();
  sendModeToLpc();
}

void RibbonParameter::sendModeToLpc() const
{
  uint16_t id = getID() == HardwareSourcesGroup::getUpperRibbonParameterID() ? PLAY_MODE_UPPER_RIBBON_BEHAVIOUR
                                                                             : PLAY_MODE_LOWER_RIBBON_BEHAVIOUR;
  uint16_t v = 0;

  if(getRibbonReturnMode() == RibbonReturnMode::RETURN)
    v += 1;

  if(getRibbonTouchBehaviour() == RibbonTouchBehaviour::RELATIVE)
    v += 2;

  if(Application::exists())
  {
    Application::get().getLPCProxy()->sendSetting(id, v);
    sendToLpc();
  }
}

void RibbonParameter::copyFrom(UNDO::Transaction *transaction, const PresetParameter *other)
{
  if(!isLocked())
  {
    super::copyFrom(transaction, other);
    undoableSetRibbonReturnMode(transaction, other->getRibbonReturnMode());
    undoableSetRibbonTouchBehaviour(transaction, other->getRibbonTouchBehaviour());
  }
}

void RibbonParameter::copyTo(UNDO::Transaction *transaction, PresetParameter *other) const
{
  super::copyTo(transaction, other);
  other->setField(transaction, PresetParameter::Fields::RibbonReturnMode, to_string(getRibbonReturnMode()));
  other->setField(transaction, PresetParameter::Fields::RibbonTouchBehaviour, to_string(getRibbonTouchBehaviour()));
}

void RibbonParameter::boundToMacroControl(tControlPositionValue v)
{
  getValue().setRawValue(Initiator::INDIRECT, v);
  onChange();
  invalidate();
}

bool RibbonParameter::hasBehavior() const
{
  return true;
}

Glib::ustring RibbonParameter::getCurrentBehavior() const
{
  switch(m_returnMode)
  {
    case RibbonReturnMode::STAY:
      return "Non-Return";

    case RibbonReturnMode::RETURN:
      return "Return Center";

    default:
      g_assert_not_reached();
      break;
  }

  throw std::logic_error("unknown ribbon return mode");
}

void RibbonParameter::undoableStepBehavior(UNDO::Transaction *transaction, int direction)
{
  auto v = static_cast<int>(m_returnMode) + direction;
  auto numModes = static_cast<int>(RibbonReturnMode::NUM_RETURN_MODES);

  while(v < 0)
    v += numModes;

  while(v >= numModes)
    v -= numModes;

  undoableSetRibbonReturnMode(transaction, static_cast<RibbonReturnMode>(v));
}

Layout *RibbonParameter::createLayout(FocusAndMode focusAndMode) const
{
  switch(focusAndMode.mode)
  {
    case UIMode::Info:
      return new ParameterInfoLayout();

    case UIMode::Edit:
      return new RibbonParameterEditLayout2();

    case UIMode::Select:
    default:
      return new RibbonParameterSelectLayout2();
  }

  g_return_val_if_reached(nullptr);
}

void RibbonParameter::loadDefault(UNDO::Transaction *transaction)
{
  super::loadDefault(transaction);
  undoableSetRibbonReturnMode(transaction, RibbonReturnMode::STAY);
  undoableSetRibbonTouchBehaviour(transaction, RibbonTouchBehaviour::ABSOLUTE);
}

size_t RibbonParameter::getHash() const
{
  size_t hash = super::getHash();
  hash_combine(hash, (int) m_touchBehaviour);
  hash_combine(hash, (int) m_returnMode);
  return hash;
}
