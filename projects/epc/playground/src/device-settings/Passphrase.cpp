#include <nltools/messaging/Messaging.h>
#include <nltools/messaging/Message.h>
#include "Passphrase.h"
#include "Settings.h"

Passphrase::Passphrase(Settings& parent, const std::shared_ptr<EpcWifi>& shrd_ptr)
    : Setting(parent)
    , m_wifi(shrd_ptr)
{
}

Passphrase::~Passphrase() = default;

void Passphrase::load(const Glib::ustring& password, Initiator initiator)
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

  auto shortened = m_password.size() <= 8 ? m_password : m_password.substr(0, 8);
  auto passwordMsg = nltools::msg::WiFi::SetWiFiPasswordMessage(shortened);
  nltools::msg::send(nltools::msg::EndPoint::BeagleBone, passwordMsg);

  m_wifi->setNewPassphrase(m_password);

  notify();
}

Glib::ustring Passphrase::getDisplayString() const
{
  return m_password;
}
