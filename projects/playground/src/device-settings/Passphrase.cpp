#include <nltools/messaging/Messaging.h>
#include <nltools/messaging/Message.h>
#include "Passphrase.h"
#include "Settings.h"

Passphrase::Passphrase(Settings& parent)
    : Setting(parent)
{
}

Passphrase::~Passphrase() = default;

void Passphrase::load(const Glib::ustring& password)
{
  updatePassword(password);
}

Glib::ustring Passphrase::save() const
{
  return m_password;
}

void Passphrase::dice()
{
  static const std::string dict = "abcdefghkmnpqrstuvwxyzABCDEFGHKMNPQRSTUVWXYZ123456789";

  std::string r;

  for(int i = 0; i < 8; i++)
    r += dict[g_random_int_range(0, dict.length())];

  updatePassword(r);
}

void Passphrase::resetToDefault()
{
  updatePassword("88888888");
}

void Passphrase::updatePassword(const Glib::ustring& password)
{
  m_password = password;

  auto passwordMsg = nltools::msg::WiFi::SetWiFiPasswordMessage(m_password);
  nltools::msg::send(nltools::msg::EndPoint::WiFiManager, passwordMsg);

  notify();
}

Glib::ustring Passphrase::getDisplayString() const
{
  return m_password;
}