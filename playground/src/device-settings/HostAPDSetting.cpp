#include <device-settings/DebugLevel.h>
#include <device-settings/HostAPDSetting.h>
#include <device-settings/Settings.h>
#include <device-settings/AccessPointRestarter.h>

#include <glibmm/fileutils.h>
#include <glibmm/ustring.h>
#include <tools/SpawnCommandLine.h>
#include <atomic>
#include <chrono>
#include <functional>
#include <sstream>
#include <string>

#if _DEVELOPMENT_PC
static const char *c_fileName = "/home/hhoegelo/development/nl/playground/hostapd.conf";
#else
static const char *c_fileName = "/etc/hostapd.conf";
#endif

HostAPDSetting::HostAPDSetting(Settings &parent, const std::string &pattern)
    : super(parent)
    , m_pattern(pattern)
{
}

HostAPDSetting::~HostAPDSetting()
{
}

void HostAPDSetting::loadFromFile()
{
  try
  {
    std::string line;
    std::stringstream stream(Glib::file_get_contents(c_fileName));

    while(std::getline(stream, line))
    {
      if(line.find(m_pattern) == 0)
      {
        m_setting = line.substr(m_pattern.size());
        break;
      }
    }
  }
  catch(...)
  {
  }
}

void HostAPDSetting::saveToFile()
{
  try
  {
    std::string line;
    std::stringstream in(Glib::file_get_contents(c_fileName));
    std::stringstream out;

    while(std::getline(in, line))
    {
      if(line.find(m_pattern) == 0)
        out << m_pattern << m_setting << std::endl;
      else
        out << line << std::endl;
    }

    Glib::file_set_contents(c_fileName, out.str());
    AccessPointRestarter::get().scheduleRestart();
  }
  catch(...)
  {
  }
}

void HostAPDSetting::load(const Glib::ustring &text)
{
}

Glib::ustring HostAPDSetting::save() const
{
  return get();
}

void HostAPDSetting::set(Glib::ustring v)
{
  if(m_setting != v)
  {
    DebugLevel::warning("host apd setting", m_pattern, "changed from", m_setting, "to", v);
    m_setting = v;
    saveToFile();
    notify();
  }
}

const Glib::ustring &HostAPDSetting::get() const
{
  return m_setting;
}
