#ifdef _DEVELOPMENT_PC
#include "EditBufferMockFactory.h"

nltools::msg::SinglePresetMessage EditBufferMockFactory::initialMessage()
{
  nltools::msg::SinglePresetMessage ret {};

  return ret;
}

nltools::msg::SplitPresetMessage EditBufferMockFactory::splitMessage()
{
  nltools::msg::SplitPresetMessage ret {};

  return ret;
}

nltools::msg::SinglePresetMessage EditBufferMockFactory::splitToSingleMessage()
{
  nltools::msg::SinglePresetMessage ret {};

  return ret;
}

nltools::msg::LayerPresetMessage EditBufferMockFactory::layerMessage()
{
  nltools::msg::LayerPresetMessage ret {};

  return ret;
}

nltools::msg::SinglePresetMessage EditBufferMockFactory::layerToSingleMessage()
{
  nltools::msg::SinglePresetMessage ret {};

  return ret;
}

#endif