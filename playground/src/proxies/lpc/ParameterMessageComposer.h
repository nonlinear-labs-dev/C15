#pragma once

#include "MessageComposer.h"

class Parameter;

class ParameterMessageComposer : public MessageComposer
{
 private:
  typedef MessageComposer super;

 public:
  // public methods
  ParameterMessageComposer(const Parameter* p);
  virtual ~ParameterMessageComposer();

  bool canReplace(const MessageComposer* other) const override;

 private:
  // private methods
  ParameterMessageComposer(const ParameterMessageComposer& other);
  ParameterMessageComposer& operator=(const ParameterMessageComposer&);

  uint16_t m_parameterID;
};
