#pragma once
#include <sigc++/connection.h>
#include "ParameterEditButtonMenu.h"

class SplitPointSyncParameters;

class SplitPointEditMenu : public ParameterEditButtonMenu
{
 public:
  explicit SplitPointEditMenu(const Rect& rect);

 protected:
  SplitPointSyncParameters* m_syncSetting;
  void addActions() override;
  sigc::connection m_syncConnection;
};