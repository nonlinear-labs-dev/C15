#include "Passphrase.h"

Passphrase::Passphrase(Settings &parent)
    : super(parent, "wpa_passphrase=")
{
  loadFromFile();
}

Passphrase::~Passphrase()
= default;

void Passphrase::dice()
{
  static const std::string dict = "abcdefghkmnpqrstuvwxyzABCDEFGHKMNPQRSTUVWXYZ123456789";

  std::string r;

  for(int i = 0; i < 8; i++)
    r += dict[g_random_int_range(0, dict.length())];

  set(r);
}

ustring Passphrase::getDisplayString() const {
  return get();
}
