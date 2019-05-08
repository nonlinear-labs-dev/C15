#pragma once

#include <proxies/lpc/MessageComposer.h>

class Preset;

class EditBufferMessageComposer : public MessageComposer
{
 private:
  using super = MessageComposer;

 public:
  EditBufferMessageComposer();
};
