#include <proxies/hwui/descriptive-layouts/ConditionRegistry.h>
#include "ConditionBase.h"

void ConditionBase::onConditionChanged() const {
  ConditionRegistry::get().onConditionChanged();
}
