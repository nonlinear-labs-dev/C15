#pragma once
#include <nltools/Types.h>
#include <device-settings/EnumSetting.h>

class MidiReceiveChannelSetting : public EnumSetting<MidiReceiveChannel>
{
 public:
  explicit MidiReceiveChannelSetting(UpdateDocumentContributor& s);
  const std::vector<Glib::ustring>& enumToString() const override;
  const std::vector<Glib::ustring>& enumToDisplayString() const override;
};

class MidiReceiveChannelSplitSetting : public EnumSetting<MidiReceiveChannelSplit>
{
 public:
  explicit MidiReceiveChannelSplitSetting(UpdateDocumentContributor& s);
  const std::vector<Glib::ustring>& enumToString() const override;
  const std::vector<Glib::ustring>& enumToDisplayString() const override;
};

class MidiSendChannelSetting : public EnumSetting<MidiSendChannel>
{
 public:
  explicit MidiSendChannelSetting(UpdateDocumentContributor& s);
  const std::vector<Glib::ustring>& enumToString() const override;
  const std::vector<Glib::ustring>& enumToDisplayString() const override;
};

class MidiSendChannelSplitSetting : public EnumSetting<MidiSendChannelSplit>
{
 public:
  explicit MidiSendChannelSplitSetting(UpdateDocumentContributor& s);
  const std::vector<Glib::ustring>& enumToString() const override;
  const std::vector<Glib::ustring>& enumToDisplayString() const override;
};