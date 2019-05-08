#pragma once

#include "playground.h"

class ValueScaling
{
 public:
  typedef gint32 tValue;

  static tValue identity(tValue in);
  static tValue linear(tValue in, tValue factor);
  static tValue expCtrl(tValue in, tValue factor);
};
