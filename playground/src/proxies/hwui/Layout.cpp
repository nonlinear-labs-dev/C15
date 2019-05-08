#include "Layout.h"

Layout::Layout()
{
}

Layout::~Layout()
{
}

void Layout::init()
{
  m_initialized = true;
  onInit();
}

void Layout::onInit()
{
}

void Layout::copyFrom(Layout *other)
{
}

bool Layout::isInitialized() const
{
  return m_initialized;
}
