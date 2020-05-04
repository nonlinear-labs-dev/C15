#pragma once
#include "MultiLineLabel.h"
#include <sigc++/connection.h>

class Parameter;

class ParameterNotAvailableInSoundInfo : public MultiLineLabel
{
 public:
  ParameterNotAvailableInSoundInfo(const Rect& r, const Glib::ustring& text);

 protected:
  void setBackgroundColor(FrameBuffer& fb) const override;

 private:
  void onSelectionChanged(const Parameter* old, const Parameter* newParam);
  void onSoundTypeChanged();

  sigc::connection m_parameterSelectionConnection;
  sigc::connection m_sountTypeConnection;
};
