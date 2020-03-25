#pragma once

#include "playground.h"
#include <vector>

class MessageParser
{
 public:
  MessageParser();
  virtual ~MessageParser();

  enum MessageTypes
  {
    PRESET_DIRECT = 0x0100,
    MORPH_TYPE_A = 0x0200,
    MORPH_TYPE_B = 0x0300,
    HARDWARE_SOURCE = 0x0400,
    EDIT_CONTROL = 0x0500,
    MORPH_POS = 0x0600,
    SETTING = 0x0700,
    NOTIFICATION = 0x0800,
    ASSERTION = 0x0900,
    REQUEST = 0x0A00,
    HEARTBEAT = 0x0B00
  };

  enum NotificationTypes
  {
    SOFTWARE_VERSION = 0x0000
  };

  struct NLMessage
  {
    NLMessage();
    ~NLMessage();

    uint16_t type;
    uint16_t length;
    std::vector<uint16_t> params;
  };

  const NLMessage &getMessage() const;
  size_t parse(const uint8_t *buffer, size_t numBytes);
  static size_t getNumInitialBytesNeeded();

 private:
  bool hasReadHeader() const;
  size_t getNumHeaderBytesMissing() const;

  size_t parseBody(const uint8_t *buffer, size_t numBytes);

  NLMessage m_msg;
  size_t m_numBytesRead;
};
