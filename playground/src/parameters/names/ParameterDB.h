#pragma once

#include <nltools/Types.h>
#include <ParameterId.h>
#include "playground.h"

class Parameter;

class ParameterDB
{
 public:
  static ParameterDB &get();

  virtual ~ParameterDB();

  Glib::ustring getLongName(const ParameterId& id) const;
  Glib::ustring getShortName(const ParameterId& id) const;
  Glib::ustring getDescription(const ParameterId& id) const;
  tControlPositionValue getSignalPathIndication(int id) const;

  static constexpr tControlPositionValue getInvalidSignalPathIndication()
  {
    return std::numeric_limits<tControlPositionValue>::max();
  }

  bool isActive(const Parameter *p) const;

 private:

  Glib::ustring replaceVoiceGroupInDynamicLabels(Glib::ustring name, VoiceGroup originGroup) const;
  ParameterDB();
};
