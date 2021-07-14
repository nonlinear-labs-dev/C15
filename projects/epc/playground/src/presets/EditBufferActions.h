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
  explicit EditBufferActions(EditBuffer* editBuffer);
  ~EditBufferActions() override = default;

  EditBufferActions(const EditBufferActions& other) = delete;
  EditBufferActions& operator=(const EditBufferActions&) = delete;
};
