#include "AftertouchCurve.h"
#include <proxies/lpc/LPCProxy.h>
#include <http/UpdateDocumentMaster.h>
#include "Application.h"
#include "proxies/lpc/LPCProxy.h"

AftertouchCurve::AftertouchCurve(Settings &settings)
    : super(settings, AftertouchCurves::AFTERTOUCH_CURVE_NORMAL)
{
}

AftertouchCurve::~AftertouchCurve()
{
}

void AftertouchCurve::sendToLPC() const
{
  Application::get().getLPCProxy()->sendSetting(AFTERTOUCH_CURVE, (uint16_t)(get()));
}

const vector<Glib::ustring> &AftertouchCurve::enumToString() const
{
  static vector<Glib::ustring> s_modeNames = { "soft", "normal", "hard" };
  return s_modeNames;
}

const vector<Glib::ustring> &AftertouchCurve::enumToDisplayString() const
{
  static vector<Glib::ustring> s_modeNames = { "Soft", "Normal", "Hard" };
  return s_modeNames;
}
