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
#include <presets/EditBuffer.h>
#include <device-settings/DebugLevel.h>
#include <cmath>

void RibbonParameter::writeDocProperties(Writer &writer, UpdateDocumentContributor::tUpdateID knownRevision) const
{
  Parameter::writeDocProperties(writer, knownRevision);
  writer.writeTextElement("ribbon-touch-behaviour", to_string(m_touchBehaviour));
  writer.writeTextElement("ribbon-return-mode", to_string(m_returnMode));
}

void RibbonParameter::writeDifferences(Writer &writer, Parameter *other) const
{
  Parameter::writeDifferences(writer, other);
  RibbonParameter *pOther = static_cast<RibbonParameter *>(other);

  if(getRibbonTouchBehaviour() != pOther->getRibbonTouchBehaviour())
  {
    writer.writeTextElement("behaviour", "", Attribute("a", getRibbonTouchBehaviour()),
                            Attribute("b", pOther->getRibbonTouchBehaviour()));
  }

  if(getReturnMode() != pOther->getReturnMode())
  {
    writer.writeTextElement("return-mode", "", Attribute("a", (int) getReturnMode()),
                            Attribute("b", (int) pOther->getReturnMode()));
  }
}

void RibbonParameter::undoableSetRibbonTouchBehaviour(UNDO::Scope::tTransactionPtr transaction,
                                                      RibbonParameter::RibbonTouchBehaviour mode)
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
  getValue().setToDefault(Initiator::INDIRECT);

  bool routersAreBoolean = getReturnMode() == ReturnMode::None;

  ParameterGroupSet *groups = dynamic_cast<ParameterGroupSet *>(getParentGroup()->getParent());
  auto mappings = dynamic_cast<MacroControlMappingGroup *>(groups->getParameterGroupByID("MCM"));
  for(auto router : mappings->getModulationRoutingParametersFor(this))
  {
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

void RibbonParameter::undoableSetRibbonTouchBehaviour(UNDO::Scope::tTransactionPtr transaction,
                                                      const Glib::ustring &mode)
{
  if(mode == "absolute")
    undoableSetRibbonTouchBehaviour(transaction, RibbonParameter::ABSOLUTE);
  else if(mode == "relative")
    undoableSetRibbonTouchBehaviour(transaction, RibbonParameter::RELATIVE);
}

void RibbonParameter::undoableIncRibbonTouchBehaviour(UNDO::Scope::tTransactionPtr transaction)
{
  int e = (int) m_touchBehaviour;
  e++;

  if(e >= NUM_TOUCH_BEHAVIOURS)
    e = 0;

  undoableSetRibbonTouchBehaviour(transaction, (RibbonParameter::RibbonTouchBehaviour) e);
}

RibbonParameter::RibbonTouchBehaviour RibbonParameter::getRibbonTouchBehaviour() const
{
  return m_touchBehaviour;
}

void RibbonParameter::undoableSetRibbonReturnMode(UNDO::Scope::tTransactionPtr transaction,
                                                  RibbonParameter::RibbonReturnMode mode)
{
  if(mode != STAY && mode != RETURN)
    mode = STAY;

  if(m_returnMode != mode)
  {
    auto swapData = UNDO::createSwapData(mode);

    transaction->addSimpleCommand([=](UNDO::Command::State) mutable {
      swapData->swapWith(m_returnMode);
      setupScalingAndDefaultValue();
    });
  }
  else
  {
    setupScalingAndDefaultValue();
  }
}

void RibbonParameter::undoableSetRibbonReturnMode(UNDO::Scope::tTransactionPtr transaction, const Glib::ustring &mode)
{
  if(mode == "stay")
    undoableSetRibbonReturnMode(transaction, STAY);
  else if(mode == "return")
    undoableSetRibbonReturnMode(transaction, RETURN);
}

RibbonParameter::RibbonReturnMode RibbonParameter::getRibbonReturnMode() const
{
  return m_returnMode;
}

void RibbonParameter::undoableIncRibbonReturnMode(UNDO::Scope::tTransactionPtr transaction)
{
  int e = (int) m_returnMode;
  e++;

  if(e >= NUM_RETURN_MODES)
    e = 0;

  undoableSetRibbonReturnMode(transaction, (RibbonParameter::RibbonReturnMode) e);
}

PhysicalControlParameter::ReturnMode RibbonParameter::getReturnMode() const
{
  if(m_returnMode == RibbonParameter::RETURN)
    return ReturnMode::Center;

  return ReturnMode::None;
}

void RibbonParameter::ensureExclusiveRoutingIfNeeded()
{
  if(getRibbonReturnMode() == STAY)
  {
    ParameterGroupSet *groups = dynamic_cast<ParameterGroupSet *>(getParentGroup()->getParent());
    auto mappings = dynamic_cast<MacroControlMappingGroup *>(groups->getParameterGroupByID("MCM"));
    auto routers = mappings->getModulationRoutingParametersFor(this);
    auto highest = *routers.begin();

    for(auto router : routers)
    {
      if(abs(router->getControlPositionValue()) > abs(highest->getControlPositionValue()))
      {
        highest = router;
      }
    }

    for(auto router : routers)
    {
      if(router != highest)
      {
        router->onExclusiveRoutingLost();
      }
    }
  }
}

bool RibbonParameter::shouldWriteDocProperties(UpdateDocumentContributor::tUpdateID knownRevision) const
{
  return Parameter::shouldWriteDocProperties(knownRevision) || knownRevision <= m_updateIdWhenModeChanged;
}

const ScaleConverter *RibbonParameter::createScaleConverter() const
{
  if(m_returnMode == RETURN)
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
  if(dynamic_cast<const EditBuffer *>(getParentGroup()->getParent()))
  {
    uint16_t id = getID() == HardwareSourcesGroup::getUpperRibbonParameterID() ? PLAY_MODE_UPPER_RIBBON_BEHAVIOUR
                                                                               : PLAY_MODE_LOWER_RIBBON_BEHAVIOUR;
    uint16_t v = 0;

    if(getRibbonReturnMode() == RibbonParameter::RibbonParameter::RETURN)
      v += 1;

    if(getRibbonTouchBehaviour() == RibbonParameter::RELATIVE)
      v += 2;

    Application::get().getLPCProxy()->sendSetting(id, v);
  }
}

void RibbonParameter::copyFrom(UNDO::Scope::tTransactionPtr transaction, Parameter *other)
{
  if(!isLocked())
  {
    super::copyFrom(transaction, other);

    if(auto ribbon = dynamic_cast<RibbonParameter *>(other))
    {
      undoableSetRibbonReturnMode(transaction, ribbon->getRibbonReturnMode());
      undoableSetRibbonTouchBehaviour(transaction, ribbon->getRibbonTouchBehaviour());
    }
  }
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
    case STAY:
      return "Non-Return";
      break;

    case RETURN:
      return "Return Center";
      break;

    default:
      g_assert_not_reached();
      break;
  }

  throw std::logic_error("unknown ribbon return mode");
}

void RibbonParameter::undoableStepBehavior(UNDO::Scope::tTransactionPtr transaction, int direction)
{
  int v = (int) m_returnMode + direction;

  while(v < 0)
    v += NUM_RETURN_MODES;

  while(v >= NUM_RETURN_MODES)
    v -= NUM_RETURN_MODES;

  undoableSetRibbonReturnMode(transaction, (RibbonParameter::RibbonReturnMode)(v));
}

DFBLayout *RibbonParameter::createLayout(FocusAndMode focusAndMode) const
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

void RibbonParameter::loadDefault(UNDO::Scope::tTransactionPtr transaction)
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
