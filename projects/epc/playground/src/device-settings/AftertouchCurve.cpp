#include "AftertouchCurve.h"
#include <proxies/playcontroller/PlaycontrollerProxy.h>
#include <http/UpdateDocumentMaster.h>
#include "Application.h"
#include "proxies/playcontroller/PlaycontrollerProxy.h"

AftertouchCurve::AftertouchCurve(UpdateDocumentContributor &settings)
    : super(settings, AftertouchCurves::AFTERTOUCH_CURVE_NORMAL)
{
}

AftertouchCurve::~AftertouchCurve()
{
}

void AftertouchCurve::syncExternals(SendReason reason) const
{
  Application::get().getPlaycontrollerProxy()->sendSetting(AFTERTOUCH_CURVE, (uint16_t)(get()));
}

const std::vector<Glib::ustring> &AftertouchCurve::enumToString() const
{
  static std::vector<Glib::ustring> s_modeNames = { "soft", "normal", "hard" };
  return s_modeNames;
}

const std::vector<Glib::ustring> &AftertouchCurve::enumToDisplayString() const
{
  static std::vector<Glib::ustring> s_modeNames = { "Soft", "Normal", "Hard" };
  return s_modeNames;
}
