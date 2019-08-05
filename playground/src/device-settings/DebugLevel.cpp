#include "DebugLevel.h"
#include "Settings.h"
#include "http/UpdateDocumentMaster.h"
#include "Application.h"
#include "proxies/lpc/LPCProxy.h"

DebugLevels lastSetLevel = DebugLevels::DEBUG_LEVEL_WARNING;

DebugLevel::DebugLevel(Settings &settings)
    : super(settings, DebugLevels::DEBUG_LEVEL_WARNING)
{
  DebugLevel::gassy(__PRETTY_FUNCTION__, G_STRLOC);
}

DebugLevel::~DebugLevel()
= default;

bool DebugLevel::set(DebugLevels m)
{
  if(super::set(m))
  {
    lastSetLevel = m;
    DebugLevel::gassy(__PRETTY_FUNCTION__, G_STRLOC, save());

    switch(m)
    {
      case DebugLevels::DEBUG_LEVEL_SILENT:
        nltools::Log::setLevel(nltools::Log::Silent);
        break;

      case DebugLevels::DEBUG_LEVEL_ERROR:
        nltools::Log::setLevel(nltools::Log::Error);
        break;

      case DebugLevels::DEBUG_LEVEL_WARNING:
        nltools::Log::setLevel(nltools::Log::Warning);
        break;

      case DebugLevels::DEBUG_LEVEL_INFO:
        nltools::Log::setLevel(nltools::Log::Info);
        break;

      case DebugLevels::DEBUG_LEVEL_DEBUG:
      case DebugLevels::DEBUG_LEVEL_GASSY:
        nltools::Log::setLevel(nltools::Log::Debug);
        break;
    }

    return true;
  }
  return false;
}

DebugLevels DebugLevel::getLevel()
{
  return lastSetLevel;
}

const std::vector<Glib::ustring> &DebugLevel::enumToString() const
{
  static std::vector<Glib::ustring> s_modeNames = { "silent", "error", "warning", "info", "debug", "gassy" };
  return s_modeNames;
}

const std::vector<Glib::ustring> &DebugLevel::enumToDisplayString() const
{
  static std::vector<Glib::ustring> s_modeNames = { "Silent", "Error", "Warning", "Info", "Debug", "Gassy" };
  return s_modeNames;
}
