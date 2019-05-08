#pragma once

#include "playground.h"

class LastLoadedPresetInfo
{
 public:
  bool operator!=(const LastLoadedPresetInfo &other) const
  {
    return presetUUID != other.presetUUID || presetName != other.presetName || bankUUID != other.bankUUID
        || bankName != other.bankName || presetDeleted != other.presetDeleted;
  }

  ustring presetUUID;
  ustring presetName;

  ustring bankUUID;
  ustring bankName;

  bool presetDeleted = false;

  friend class LastLoadedPresetInfoSerializer;
};
