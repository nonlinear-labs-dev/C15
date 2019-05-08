#include "Application.h"
#include "UpperRibbon.h"
#include "parameters/Parameter.h"
#include <parameters/RibbonParameter.h>
#include "presets/EditBuffer.h"
#include "presets/PresetSettings.h"
#include "presets/PresetManager.h"
#include "device-settings/BaseUnitUIMode.h"
#include "device-settings/Settings.h"
#include "device-settings/DebugLevel.h"
#include "groups/HardwareSourcesGroup.h"
#include <glib.h>
#include <math.h>

// static TestDriver<UpperRibbon> tests;

UpperRibbon::UpperRibbon()
{
  initLEDs();

  Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
      sigc::mem_fun(this, &UpperRibbon::onParamSelectionChanged));
  Application::get().getSettings()->getSetting<BaseUnitUIMode>()->onChange(
      sigc::mem_fun(this, &UpperRibbon::onSettingChanged));
}

UpperRibbon::~UpperRibbon()
{
}

void UpperRibbon::onParamSelectionChanged(Parameter* oldOne, Parameter* newOne)
{
  reconnect();
}

void UpperRibbon::onSettingChanged(const Setting* setting)
{
  reconnect();
}

void UpperRibbon::reconnect()
{
  m_paramConnection.disconnect();

  if(auto p = getResponsibleParameter())
    m_paramConnection = p->onParameterChanged(sigc::mem_fun(this, &UpperRibbon::onParamValueChanged));
}

Parameter* UpperRibbon::getResponsibleParameter()
{
  auto s = Application::get().getSettings()->getSetting<BaseUnitUIMode>();

  if(s->get() == BaseUnitUIModes::ParameterEdit)
    return Application::get().getPresetManager()->getEditBuffer()->getSelectedParameter();

  return Application::get().getPresetManager()->getEditBuffer()->findParameterByID(
      HardwareSourcesGroup::getUpperRibbonParameterID());
}

void UpperRibbon::onParamValueChanged(const Parameter* param)
{
  bool bipol = false;

  auto paramVal = param->getControlPositionValue();
  auto s = Application::get().getSettings()->getSetting<BaseUnitUIMode>();

  if(s->get() == BaseUnitUIModes::ParameterEdit)
  {
    bipol = param->isBiPolar();
  }
  else  // BASE_UNIT_UI_MODE_PLAY
  {
    if(auto ribbonParameter = dynamic_cast<const RibbonParameter*>(param))
    {
      bipol = ribbonParameter->getRibbonReturnMode() == RibbonParameter::RETURN;
    }
  }

  if(!bipol)
    setLEDsForValueUniPolar(paramVal, false);
  else
    setLEDsForValueBiPolar(paramVal, false);
}

int UpperRibbon::posToLedID(int pos) const
{
  return 2 * pos + 1;
}

void UpperRibbon::registerTests()
{
#if 0
  class TestRibbon : public UpperRibbon
  {
    public:
    int leds[NUM_LEDS_PER_RIBBON];

    TestRibbon ()
    {
      resetLEDs ();
    }

    protected:
    void setLEDState (int ledPos, char state)
    {
      g_assert (ledPos >= 0);
      g_assert (ledPos < NUM_LEDS_PER_RIBBON);
      g_assert (state >= 0);
      g_assert (state <= 3);

      leds[ledPos] = state;
    }
  };

  g_test_add_func ("/UpperRibbon/uni/zero", []()
      {
        TestRibbon r;
        r.setLEDsForValueUniPolar(0.0f);

        for(int i = 0; i < NUM_LEDS_PER_RIBBON; i++)
        g_assert(r.leds[i] == 0);
      });

  g_test_add_func ("/UpperRibbon/uni/one", []()
      {
        TestRibbon r;
        r.setLEDsForValueUniPolar(1.0f);

        for(int i = 0; i < NUM_LEDS_PER_RIBBON; i++)
        g_assert(r.leds[i] == 3);
      });

  g_test_add_func ("/UpperRibbon/uni/99", []()
      {
        TestRibbon r;
        r.setLEDsUniPolar(99);

        for(int i = 0; i < NUM_LEDS_PER_RIBBON - 1; i++)
        g_assert(r.leds[i] == 3);

        g_assert(r.leds[NUM_LEDS_PER_RIBBON - 1] < 3);
      });

  g_test_add_func ("/UpperRibbon/uni/1", []()
      {
        TestRibbon r;
        r.setLEDsUniPolar(1);

        for(int i = 1; i < NUM_LEDS_PER_RIBBON; i++)
        g_assert(r.leds[i] == 0);

        g_assert(r.leds[0] > 0);
      });

  g_test_add_func ("/UpperRibbon/uni/half", []()
      {
        TestRibbon r;
        r.setLEDsForValueUniPolar(0.5f);

        for(int i = 0; i < 16; i++)
        g_assert(r.leds[i] == 3);

        for(int i = 17; i < NUM_LEDS_PER_RIBBON; i++)
        g_assert(r.leds[i] == 0);

        g_assert(r.leds[16] == 1);
      });

  g_test_add_func ("/UpperRibbon/bi/zero", []()
      {
        TestRibbon r;
        int centerLED = (NUM_LEDS_PER_RIBBON / 2);
        r.setLEDsForValueBiPolar(0.0f);

        for(int i = 0; i < NUM_LEDS_PER_RIBBON; i++)
        g_assert(r.leds[i] == 0 || i == centerLED);

        g_assert(r.leds[centerLED] == 1);
      });

  g_test_add_func ("/UpperRibbon/bi/+1", []()
      {
        TestRibbon r;
        r.setLEDsForValueBiPolar(1.0f);

        for(int i = 0; i < NUM_LEDS_PER_RIBBON / 2; i++)
        g_assert(r.leds[i] == 0);

        for(int i = NUM_LEDS_PER_RIBBON / 2; i < NUM_LEDS_PER_RIBBON; i++)
        g_assert(r.leds[i] == 3);
      });

  g_test_add_func ("/UpperRibbon/bi/-1", []()
      {
        TestRibbon r;
        r.setLEDsForValueBiPolar(-1.0f);

        for(int i = 0; i < NUM_LEDS_PER_RIBBON / 2 + 1; i++)
        g_assert(r.leds[i] == 3);

        for(int i = NUM_LEDS_PER_RIBBON / 2 + 1; i < NUM_LEDS_PER_RIBBON; i++)
        g_assert(r.leds[i] == 0);
      });

  g_test_add_func ("/UpperRibbon/bi/+0.5", []()
      {
        TestRibbon r;
        r.setLEDsForValueBiPolar(0.5f);

        g_assert(NUM_LEDS_PER_RIBBON == 33);

        for(int i = 0; i < 16; i++)
        g_assert(r.leds[i] == 0);

        for(int i = 16; i < 23; i++)
        g_assert(r.leds[i] == 3);

        g_assert(r.leds[24] == 1 || r.leds[24] == 2);

        for(int i = 25; i < 23; i++)
        g_assert(r.leds[i] == 0);
      });

  g_test_add_func ("/UpperRibbon/bi/-0.5", []()
      {
        TestRibbon r;
        r.setLEDsForValueBiPolar(-0.5f);

        g_assert(NUM_LEDS_PER_RIBBON == 33);

        for(int i = 0; i < 7; i++)
        g_assert(r.leds[i] == 0);

        g_assert(r.leds[8] == 1 || r.leds[8] == 2);

        for(int i = 9; i < 17; i++)
        g_assert(r.leds[i] == 3);

        for(int i = 17; i < 33; i++)
        g_assert(r.leds[i] == 0);
      });
#endif
}
