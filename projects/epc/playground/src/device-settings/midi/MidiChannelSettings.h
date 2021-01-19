#pragma once
#include <nltools/Types.h>
#include <device-settings/NLEnumSetting.h>

class MidiReceiveChannelSetting : public NLEnumSetting<MidiReceiveChannel>
{
 public:
  explicit MidiReceiveChannelSetting(UpdateDocumentContributor& s);
};

class MidiReceiveChannelSplitSetting : public NLEnumSetting<MidiReceiveChannelSplit>
{
 public:
  explicit MidiReceiveChannelSplitSetting(UpdateDocumentContributor& s);
};

class MidiSendChannelSetting : public NLEnumSetting<MidiSendChannel>
{
 public:
  explicit MidiSendChannelSetting(UpdateDocumentContributor& s);
};

class MidiSendChannelSplitSetting : public NLEnumSetting<MidiSendChannelSplit>
{
 public:
  explicit MidiSendChannelSplitSetting(UpdateDocumentContributor& s);
};