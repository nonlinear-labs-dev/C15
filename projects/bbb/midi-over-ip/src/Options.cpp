#include "Options.h"
#include <nltools/logging/Log.h>
#include <nltools/ErrorCodes.h>
#include <glibmm.h>

namespace OptionDetail
{
  template <typename T>
  inline Glib::OptionEntry createOption(Glib::OptionGroup& ctx, const char* l, char s, const char* desc, T& dest)
  {
    Glib::OptionEntry entry;
    entry.set_long_name(l);
    entry.set_short_name(s);
    entry.set_description(desc);
    ctx.add_entry(entry, dest);
    return entry;
  }
}

Options::Options(int& argc, char**& argv)
{
  Glib::OptionGroup mainGroup("common", "common options");
  Glib::OptionContext ctx;

  OptionDetail::createOption(mainGroup, "audioengine-host", 'a', "where to find the audio engine", m_audioengineHost);
  ctx.set_main_group(mainGroup);
  ctx.set_help_enabled(true);

  try
  {
    ctx.parse(argc, argv);
  }
  catch(const Glib::OptionError& optErr)
  {
    nltools::Log::error(optErr.what());
    std::exit(static_cast<int>(nltools::ErrorCode::UnknownOptionKeyError));
  }
  catch(const Glib::ConvertError& convErr)
  {
    nltools::Log::error(convErr.what());
    std::exit(static_cast<int>(nltools::ErrorCode::OptionConvertError));
  }
  catch(...)
  {
    std::stringstream ss;
    for(auto i = 0; i < argc; i++)
      ss << argv[i] << " ";
    nltools::Log::error(__FILE__, __FUNCTION__, __LINE__, "Could not parse args:", ss.str());
    std::exit(static_cast<int>(nltools::ErrorCode::UnknownOptionKeyError));
  }
}

Glib::ustring Options::getAudioEngineHost() const
{
  return m_audioengineHost;
}
