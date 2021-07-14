#pragma once

#include "EnumSetting.h"
#include <nltools/Types.h>

class AftertouchCurve : public EnumSetting<AftertouchCurves>
{
 private:
  typedef EnumSetting<AftertouchCurves> super;

 public:
  explicit AftertouchCurve(UpdateDocumentContributor &settings);
  ~AftertouchCurve() override;

  void syncExternals(SendReason reason) const override;

  const std::vector<Glib::ustring> &enumToDisplayString() const override;

 private:
  AftertouchCurve(const AftertouchCurve &other);
  AftertouchCurve &operator=(const AftertouchCurve &);

  const std::vector<Glib::ustring> &enumToString() const override;
};
