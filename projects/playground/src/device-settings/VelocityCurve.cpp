#include "VelocityCurve.h"
#include <proxies/lpc/LPCProxy.h>
#include <http/UpdateDocumentMaster.h>
#include "Application.h"
#include "proxies/lpc/LPCProxy.h"

VelocityCurve::VelocityCurve(UpdateDocumentContributor &settings)
    : super(settings, VelocityCurves::VELOCITY_CURVE_NORMAL)
{
}

VelocityCurve::~VelocityCurve()
{
}

void VelocityCurve::sendToLPC(SendReason reason) const
{
  Application::get().getLPCProxy()->sendSetting(VELOCITY_CURVE, (uint16_t)(get()));
}

const std::vector<Glib::ustring> &VelocityCurve::enumToString() const
{
  static std::vector<Glib::ustring> s_modeNames = { "very-soft", "soft", "normal", "hard", "very-hard" };
  return s_modeNames;
}

const std::vector<Glib::ustring> &VelocityCurve::enumToDisplayString() const
{
  static std::vector<Glib::ustring> s_modeNames = { "Very Soft", "Soft", "Normal", "Hard", "Very Hard" };
  return s_modeNames;
}
