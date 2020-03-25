#include <Application.h>
#include <device-settings/PedalType.h>
#include <device-settings/Settings.h>
#include <groups/HardwareSourcesGroup.h>
#include <groups/MacroControlMappingGroup.h>
#include <libundo/undo/Scope.h>
#include <libundo/undo/SwapData.h>
#include <libundo/undo/Transaction.h>
#include <parameters/ModulationRoutingParameter.h>
#include <parameters/PedalParameter.h>
#include <parameters/scale-converters/Linear100PercentScaleConverter.h>
#include <parameters/scale-converters/LinearBipolar100PercentScaleConverter.h>
#include <parameters/scale-converters/ScaleConverter.h>
#include <parameters/value/QuantizedValue.h>
#include <playground.h>
#include <presets/ParameterDualGroupSet.h>
#include <presets/PresetParameter.h>
#include <proxies/hwui/HWUIEnums.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/ParameterInfoLayout.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/PlayControlParameterLayouts.h>
#include <proxies/lpc/LPCProxy.h>
#include <xml/Writer.h>
#include <presets/EditBuffer.h>

void PedalParameter::writeDocProperties(Writer &writer, UpdateDocumentContributor::tUpdateID knownRevision) const
{
  Parameter::writeDocProperties(writer, knownRevision);
  writer.writeTextElement("pedal-mode", to_string(m_mode));
}

void PedalParameter::undoableSetPedalMode(UNDO::Transaction *transaction, PedalModes mode)
{
  if(mode != PedalModes::STAY && mode != PedalModes::RETURN_TO_ZERO && mode != PedalModes::RETURN_TO_CENTER)
    mode = PedalModes::STAY;

  if(m_mode != mode)
  {
    auto swapData = UNDO::createSwapData(mode);

    transaction->addSimpleCommand([=](UNDO::Command::State) mutable {
      swapData->swapWith(m_mode);
      getValue().setScaleConverter(createScaleConverter());
      auto defValue = getDefValueAccordingToMode();
      getValue().setDefaultValue(defValue);
      getValue().setToDefault(Initiator::INDIRECT);

      setRoutersModeAccordingToReturnMode();

      invalidate();
      sendModeToLpc();
      m_updateIdWhenModeChanged = getUpdateIDOfLastChange();
    });
  }
  else
  {
    setRoutersModeAccordingToReturnMode();
    sendModeToLpc();
  }
}

void PedalParameter::setRoutersModeAccordingToReturnMode()
{
  bool routersAreBoolean = getReturnMode() == ReturnMode::None;
  if(auto eb = dynamic_cast<ParameterDualGroupSet *>(getParentGroup()->getParent()))
  {
    auto mappings = dynamic_cast<MacroControlMappingGroup *>(eb->getParameterGroupByID({ "MCM", VoiceGroup::Global }));

    for(auto router : mappings->getModulationRoutingParametersFor(this))
      router->getValue().setIsBoolean(routersAreBoolean);
  }
}

tControlPositionValue PedalParameter::getDefValueAccordingToMode() const
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

bool PedalParameter::shouldWriteDocProperties(UpdateDocumentContributor::tUpdateID knownRevision) const
{
  return Parameter::shouldWriteDocProperties(knownRevision) || knownRevision <= m_updateIdWhenModeChanged;
}

const ScaleConverter *PedalParameter::createScaleConverter() const
{
  if(m_mode == PedalModes::RETURN_TO_CENTER)
    return ScaleConverter::get<LinearBipolar100PercentScaleConverter>();

  return ScaleConverter::get<Linear100PercentScaleConverter>();
}

void PedalParameter::undoableSetPedalMode(UNDO::Transaction *transaction, const Glib::ustring &mode)
{
  if(mode == "stay")
    undoableSetPedalMode(transaction, PedalModes::STAY);
  else if(mode == "return-to-zero")
    undoableSetPedalMode(transaction, PedalModes::RETURN_TO_ZERO);
  else if(mode == "return-to-center")
    undoableSetPedalMode(transaction, PedalModes::RETURN_TO_CENTER);
}

void PedalParameter::undoableIncPedalMode(UNDO::Transaction *transaction)
{
  int e = (int) m_mode;
  e++;

  if(e >= static_cast<int>(PedalModes::NUM_PEDAL_MODES))
    e = 0;

  undoableSetPedalMode(transaction, static_cast<PedalModes>(e));
}

void PedalParameter::onPresetSentToLpc() const
{
  Parameter::onPresetSentToLpc();
  sendModeToLpc();
}

void PedalParameter::sendModeToLpc() const
{
  uint16_t id = mapParameterIdToLPCSetting();
  uint16_t v = (uint16_t) m_mode;
  Application::get().getLPCProxy()->sendSetting(id, v);

  sendToLpc();
}

uint16_t PedalParameter::mapParameterIdToLPCSetting() const
{
  if(getID() == HardwareSourcesGroup::getPedal1ParameterID())
    return LPCSettingIDs::PEDAL_1_MODE;
  else if(getID() == HardwareSourcesGroup::getPedal2ParameterID())
    return LPCSettingIDs::PEDAL_2_MODE;
  else if(getID() == HardwareSourcesGroup::getPedal3ParameterID())
    return LPCSettingIDs::PEDAL_3_MODE;
  else if(getID() == HardwareSourcesGroup::getPedal4ParameterID())
    return LPCSettingIDs::PEDAL_4_MODE;

  throw std::runtime_error("Pedal parameter has no pedal parameter id!?");
}

ReturnMode PedalParameter::getReturnMode() const
{
  switch(m_mode)
  {
    case PedalModes::STAY:
      return ReturnMode::None;

    case PedalModes::RETURN_TO_CENTER:
      return ReturnMode::Center;

    case PedalModes::RETURN_TO_ZERO:
      return ReturnMode::Zero;
  }

  return ReturnMode::None;
}

PedalModes PedalParameter::getPedalMode() const
{
  return m_mode;
}

void PedalParameter::copyFrom(UNDO::Transaction *transaction, const PresetParameter *other)
{
  if(!isLocked())
  {
    super::copyFrom(transaction, other);
    undoableSetPedalMode(transaction, other->getPedalMode());
  }
}

void PedalParameter::copyTo(UNDO::Transaction *transaction, PresetParameter *other) const
{
  super::copyTo(transaction, other);
  other->setField(transaction, PresetParameter::Fields::PedalMode, to_string(getPedalMode()));
}

bool PedalParameter::hasBehavior() const
{
  return true;
}

Glib::ustring PedalParameter::getCurrentBehavior() const
{
  switch(m_mode)
  {
    case PedalModes::STAY:
      return "Non-Return";

    case PedalModes::RETURN_TO_ZERO:
      return "Return Zero";

    case PedalModes::RETURN_TO_CENTER:
      return "Return Center";
  }

  return PhysicalControlParameter::getCurrentBehavior();
}

void PedalParameter::undoableStepBehavior(UNDO::Transaction *transaction, int direction)
{
  int e = static_cast<int>(m_mode);
  e += direction;

  if(e >= static_cast<int>(PedalModes::NUM_PEDAL_MODES))
    e = 0;
  else if(e < 0)
    e = static_cast<int>(PedalModes::RETURN_TO_CENTER);

  undoableSetPedalMode(transaction, static_cast<PedalModes>(e));
}

Layout *PedalParameter::createLayout(FocusAndMode focusAndMode) const
{
  switch(focusAndMode.mode)
  {
    case UIMode::Info:
      return new ParameterInfoLayout();

    case UIMode::Edit:
      return new PedalParameterEditLayout2();

    case UIMode::Select:
    default:
      return new PedalParameterSelectLayout2();
  }

  return super::createLayout(focusAndMode);
}

std::shared_ptr<PedalType> PedalParameter::getAssociatedPedalTypeSetting() const
{
  int key = 1;

  if(getID() == HardwareSourcesGroup::getPedal2ParameterID())
    key = 2;
  else if(getID() == HardwareSourcesGroup::getPedal3ParameterID())
    key = 3;
  else if(getID() == HardwareSourcesGroup::getPedal4ParameterID())
    key = 4;

  auto str = Glib::ustring::format("Pedal", key, "Type");
  return std::dynamic_pointer_cast<PedalType>(Application::get().getSettings()->getSetting(str));
}

void PedalParameter::loadDefault(UNDO::Transaction *transaction)
{
  super::loadDefault(transaction);
  undoableSetPedalMode(transaction, PedalModes::STAY);
}

size_t PedalParameter::getHash() const
{
  size_t hash = super::getHash();
  hash_combine(hash, (int) m_mode);
  return hash;
}
