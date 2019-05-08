#pragma once

#include "http/RPCActionManager.h"

class EditBuffer;

class EditBufferActions : public RPCActionManager
{
 private:
  typedef RPCActionManager super;

 public:
  EditBufferActions(shared_ptr<EditBuffer> editBuffer);
  virtual ~EditBufferActions();

 private:
  EditBufferActions(const EditBufferActions& other) = delete;
  EditBufferActions& operator=(const EditBufferActions&) = delete;
};
