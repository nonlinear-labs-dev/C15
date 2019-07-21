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
{
}

bool DebugLevel::set(DebugLevels m)
{
  if(super::set(m))
  {
    lastSetLevel = m;
    DebugLevel::gassy(__PRETTY_FUNCTION__, G_STRLOC, save());

    switch(m)
    {
      case DebugLevels::DEBUG_LEVEL_DEBUG:
        nltools::Log::setLevel(nltools::Log::Debug);
        break;

      case DebugLevels::DEBUG_LEVEL_SILENT:
      case DebugLevels::DEBUG_LEVEL_ERROR:
        nltools::Log::setLevel(nltools::Log::Error);
        break;

      case DebugLevels::DEBUG_LEVEL_WARNING:
        nltools::Log::setLevel(nltools::Log::Warning);
        break;

      case DebugLevels::DEBUG_LEVEL_INFO:
        nltools::Log::setLevel(nltools::Log::Info);
        break;

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
  static std::vector<Glib::ustring> s_modeNames = { "debug", "silent", "error", "warning", "info", "gassy" };
  return s_modeNames;
}

const std::vector<Glib::ustring> &DebugLevel::enumToDisplayString() const
{
  static std::vector<Glib::ustring> s_modeNames = { "Debug", "Silent", "Error", "Warning", "Info", "Gassy" };
  return s_modeNames;
}
