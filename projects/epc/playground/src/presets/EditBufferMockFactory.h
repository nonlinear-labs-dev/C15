#pragma once
#ifdef _DEVELOPMENT_PC
#include <nltools/messaging/Message.h>

class EditBufferMockFactory
{
 public:
  static nltools::msg::SinglePresetMessage initialMessage();
  static nltools::msg::SplitPresetMessage splitMessage();
  static nltools::msg::SinglePresetMessage splitToSingleMessage();
  static nltools::msg::LayerPresetMessage layerMessage();
  static nltools::msg::SinglePresetMessage layerToSingleMessage();
};
#endif
