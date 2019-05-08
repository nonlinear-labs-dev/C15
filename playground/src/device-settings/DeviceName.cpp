#include <device-settings/DeviceName.h>
#include <device-settings/Settings.h>
#include <libundo/undo/StringTools.h>
#include <set>
#include <tools/TimeTools.h>

DeviceName::DeviceName(Settings &parent)
    : super(parent)
{
}

DeviceName::~DeviceName()
{
}

void DeviceName::load(const Glib::ustring &text)
{
  set(text);
}

Glib::ustring DeviceName::save() const
{
  return get();
}

void DeviceName::set(Glib::ustring name)
{
  if(m_name != name || m_name.empty())
  {
    m_name = name;
    sanitize();
    notify();
  }
}

const Glib::ustring &DeviceName::get() const
{
  return m_name;
}

void DeviceName::sanitize()
{
  if(m_name.empty())
  {
    guint16 u = TimeTools::getAdjustedTimestamp();

    try
    {
      std::hash<std::string> h;
      std::string out = Glib::file_get_contents("/sys/class/net/wlan0/address");
      u = (guint16) h(out);
    }
    catch(...)
    {
    }

    char txt[256];
    snprintf(txt, 256, "Unit-%05d", u);
    set(txt);
  }
}
