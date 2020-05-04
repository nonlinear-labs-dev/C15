#pragma once
#include "BooleanSetting.h"

class DirectLoadSetting : public BooleanSetting
{
 public:
  explicit DirectLoadSetting(Settings& settings);

  DirectLoadSetting(const DirectLoadSetting& other) = delete;
  DirectLoadSetting& operator=(const DirectLoadSetting&) = delete;

  bool set(tEnum m) override;
  void load(const Glib::ustring& text) override;

 private:
  bool m_isLoading = false;
};
