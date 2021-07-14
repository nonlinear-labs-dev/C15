#pragma once
#include <device-settings/Settings.h>

class TestSettings : public Settings
{
 public:
  TestSettings();
  void handleHTTPRequest(std::shared_ptr<NetworkRequest> request, const Glib::ustring &path) override;
};
