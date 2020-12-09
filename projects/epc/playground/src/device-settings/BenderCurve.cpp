#include <device-settings/BenderCurve.h>
#include <proxies/playcontroller/PlaycontrollerProxy.h>
#include <http/UpdateDocumentMaster.h>
#include "Application.h"
#include "proxies/playcontroller/PlaycontrollerProxy.h"

BenderCurve::BenderCurve(UpdateDocumentContributor &settings)
    : super(settings, BenderCurves::Soft)
{
}

BenderCurve::~BenderCurve()
{
}

void BenderCurve::syncExternals(SendReason reason) const
{
  Application::get().getPlaycontrollerProxy()->sendSetting(BENDER_CURVE, (uint16_t)(get()));
}

const std::vector<Glib::ustring> &BenderCurve::enumToString() const
{
  static std::vector<Glib::ustring> s_modeNames = { "soft", "normal", "hard" };
  return s_modeNames;
}

const std::vector<Glib::ustring> &BenderCurve::enumToDisplayString() const
{
  static std::vector<Glib::ustring> s_modeNames = { "Soft", "Normal", "Hard" };
  return s_modeNames;
}
