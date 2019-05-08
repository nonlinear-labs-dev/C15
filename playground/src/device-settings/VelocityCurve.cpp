#include "VelocityCurve.h"
#include <proxies/lpc/LPCProxy.h>
#include <http/UpdateDocumentMaster.h>
#include "Application.h"
#include "proxies/lpc/LPCProxy.h"

VelocityCurve::VelocityCurve(Settings &settings)
    : super(settings, VelocityCurves::VELOCITY_CURVE_NORMAL)
{
}

VelocityCurve::~VelocityCurve()
{
}

void VelocityCurve::sendToLPC() const
{
  Application::get().getLPCProxy()->sendSetting(VELOCITY_CURVE, (uint16_t)(get()));
}

const vector<Glib::ustring> &VelocityCurve::enumToString() const
{
  static vector<Glib::ustring> s_modeNames = { "very-soft", "soft", "normal", "hard", "very-hard" };
  return s_modeNames;
}

const vector<Glib::ustring> &VelocityCurve::enumToDisplayString() const
{
  static vector<Glib::ustring> s_modeNames = { "Very Soft", "Soft", "Normal", "Hard", "Very Hard" };
  return s_modeNames;
}
