#pragma once

#include <parameters/Parameter.h>

template <typename T> class TestParameter : public T
{
 public:
  template <typename... tArgs>
  explicit TestParameter(tArgs... args)
      : T { args... } {};

 protected:
  void sendToPlaycontroller() const override {};
};
