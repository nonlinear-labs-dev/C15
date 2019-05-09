#pragma once

#include "http/RPCActionManager.h"

class EditBuffer;
class Parameter;

class EditBufferActions : public RPCActionManager
{
 private:
  typedef RPCActionManager super;

 public:
  typedef Parameter* tParameterPtr;
  EditBufferActions(EditBuffer* editBuffer);
  virtual ~EditBufferActions() = default;

 private:
  EditBufferActions(const EditBufferActions& other) = delete;
  EditBufferActions& operator=(const EditBufferActions&) = delete;
};
