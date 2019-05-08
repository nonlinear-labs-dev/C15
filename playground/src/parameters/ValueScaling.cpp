#include "ValueScaling.h"

ValueScaling::tValue ValueScaling::identity(tValue in)
{
  return in;
}

ValueScaling::tValue ValueScaling::linear(tValue in, tValue factor)
{
  return in * factor;
}

ValueScaling::tValue ValueScaling::expCtrl(tValue in, tValue factor)
{
  tValue exp = in >> 4;
  tValue rest = in - exp * 16;
  tValue basis = factor << exp;
  return (basis + ((rest * basis) >> 4));
}
