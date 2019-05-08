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
#include <presets/EditBuffer.h>
#include <proxies/hwui/HWUIEnums.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/ParameterInfoLayout.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/PlayControlParameterLayouts.h>
#include <proxies/lpc/LPCProxy.h>
#include <xml/Writer.h>

void PedalParameter::writeDocProperties(Writer &writer, UpdateDocumentContributor::tUpdateID knownRevision) const
{
  Parameter::writeDocProperties(writer, knownRevision);
  writer.writeTextElement("pedal-mode", to_string(m_mode));
}

void PedalParameter::writeDifferences(Writer &writer, Parameter *other) const
{
  Parameter::writeDifferences(writer, other);
  PedalParameter *pOther = static_cast<PedalParameter *>(other);

  if(getReturnMode() != pOther->getReturnMode())
  {
    writer.writeTextElement("return-mode", "", Attribute("a", (int) getReturnMode()),
                            Attribute("b", (int) pOther->getReturnMode()));
  }
}

void PedalParameter::undoableSetPedalMode(UNDO::Scope::tTransactionPtr transaction, PedalModes mode)
{
  if(mode != STAY && mode != RETURN_TO_ZERO && mode != RETURN_TO_CENTER)
    mode = STAY;

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
  ParameterGroupSet *groups = dynamic_cast<ParameterGroupSet *>(getParentGroup()->getParent());
  auto mappings = dynamic_cast<MacroControlMappingGroup *>(groups->getParameterGroupByID("MCM"));
  for(auto router : mappings->getModulationRoutingParametersFor(this))
  {
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
  if(m_mode == RETURN_TO_CENTER)
    return ScaleConverter::get<LinearBipolar100PercentScaleConverter>();

  return ScaleConverter::get<Linear100PercentScaleConverter>();
}

void PedalParameter::undoableSetPedalMode(UNDO::Scope::tTransactionPtr transaction, const Glib::ustring &mode)
{
  if(mode == "stay")
    undoableSetPedalMode(transaction, PedalParameter::STAY);
  else if(mode == "return-to-zero")
    undoableSetPedalMode(transaction, PedalParameter::RETURN_TO_ZERO);
  else if(mode == "return-to-center")
    undoableSetPedalMode(transaction, PedalParameter::RETURN_TO_CENTER);
}

void PedalParameter::undoableIncPedalMode(UNDO::Scope::tTransactionPtr transaction)
{
  int e = (int) m_mode;
  e++;

  if(e >= NUM_PEDAL_MODES)
    e = 0;

  undoableSetPedalMode(transaction, (PedalParameter::PedalModes) e);
}

void PedalParameter::onPresetSentToLpc() const
{
  Parameter::onPresetSentToLpc();
  sendModeToLpc();
}

void PedalParameter::sendModeToLpc() const
{
  if(dynamic_cast<const EditBuffer *>(getParentGroup()->getParent()))
  {
    uint16_t id = mapParameterIdToLPCSetting();
    uint16_t v = (uint16_t) m_mode;
    Application::get().getLPCProxy()->sendSetting(id, v);
  }
}

uint16_t PedalParameter::mapParameterIdToLPCSetting() const
{
  switch(getID())
  {
    case HardwareSourcesGroup::getPedal1ParameterID():
      return LPCSettingIDs::PEDAL_1_MODE;

    case HardwareSourcesGroup::getPedal2ParameterID():
      return LPCSettingIDs::PEDAL_2_MODE;

    case HardwareSourcesGroup::getPedal3ParameterID():
      return LPCSettingIDs::PEDAL_3_MODE;

    case HardwareSourcesGroup::getPedal4ParameterID():
      return LPCSettingIDs::PEDAL_4_MODE;
  }

  throw exception();
}

PhysicalControlParameter::ReturnMode PedalParameter::getReturnMode() const
{
  switch(m_mode)
  {
    case STAY:
      return ReturnMode::None;

    case RETURN_TO_CENTER:
      return ReturnMode::Center;

    case RETURN_TO_ZERO:
      return ReturnMode::Zero;
  }

  return ReturnMode::None;
}

PedalParameter::PedalModes PedalParameter::getPedalMode() const
{
  return m_mode;
}

void PedalParameter::copyFrom(UNDO::Scope::tTransactionPtr transaction, Parameter *other)
{
  if(!isLocked())
  {
    super::copyFrom(transaction, other);

    if(auto pedal = dynamic_cast<PedalParameter *>(other))
    {
      undoableSetPedalMode(transaction, pedal->getPedalMode());
    }
  }
}

bool PedalParameter::hasBehavior() const
{
  return true;
}

Glib::ustring PedalParameter::getCurrentBehavior() const
{
  switch(m_mode)
  {
    case STAY:
      return "Non-Return";

    case RETURN_TO_ZERO:
      return "Return Zero";

    case RETURN_TO_CENTER:
      return "Return Center";
  }

  return PhysicalControlParameter::getCurrentBehavior();
}

void PedalParameter::undoableStepBehavior(UNDO::Scope::tTransactionPtr transaction, int direction)
{
  int e = (int) m_mode;
  e += direction;

  if(e >= NUM_PEDAL_MODES)
    e = 0;
  else if(e < 0)
    e = RETURN_TO_CENTER;

  undoableSetPedalMode(transaction, (PedalParameter::PedalModes) e);
}

DFBLayout *PedalParameter::createLayout(FocusAndMode focusAndMode) const
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

shared_ptr<PedalType> PedalParameter::getAssociatedPedalTypeSetting() const
{
  int key = 1;

  if(getID() == HardwareSourcesGroup::getPedal2ParameterID())
    key = 2;
  else if(getID() == HardwareSourcesGroup::getPedal3ParameterID())
    key = 3;
  else if(getID() == HardwareSourcesGroup::getPedal4ParameterID())
    key = 4;

  auto str = ustring::format("Pedal", key, "Type");
  return dynamic_pointer_cast<PedalType>(Application::get().getSettings()->getSetting(str));
}

void PedalParameter::loadDefault(UNDO::Scope::tTransactionPtr transaction)
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
