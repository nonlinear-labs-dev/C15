#pragma once

#include <tools/DelayedJob.h>
#include "InvertedLabel.h"

class Application;
class Parameter;

class AnyParameterLockedIndicator : public InvertedLabel
{
  private:
    typedef InvertedLabel super;

  public:
    AnyParameterLockedIndicator (const Rect &pos);
    virtual ~AnyParameterLockedIndicator ();

  private:
    void calcHasLocks();
    void update ();

    DelayedJob m_calcHasLocks;
};

