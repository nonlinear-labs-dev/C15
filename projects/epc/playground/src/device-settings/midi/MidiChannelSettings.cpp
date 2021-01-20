#include <tools/StringTools.h>
#include "MidiChannelSettings.h"

MidiReceiveChannelSetting::MidiReceiveChannelSetting(UpdateDocumentContributor& s)
    : EnumSetting<MidiReceiveChannel>(s, MidiReceiveChannel::None)
{
}

const std::vector<Glib::ustring>& MidiReceiveChannelSetting::enumToString() const
{
  static auto ret = StringTools::convert(getAllStrings<MidiReceiveChannel>());
  return ret;
}

const std::vector<Glib::ustring>& MidiReceiveChannelSetting::enumToDisplayString() const
{
  static std::vector<Glib::ustring> ret { "None", "Omni", "1",  "2",  "3",  "4",  "5",  "6",  "7",
                                          "8",    "9",    "10", "11", "12", "13", "14", "15", "16" };
  return ret;
}

MidiReceiveChannelSplitSetting::MidiReceiveChannelSplitSetting(UpdateDocumentContributor& s)
    : EnumSetting<MidiReceiveChannelSplit>(s, MidiReceiveChannelSplit::None)
{
}

const std::vector<Glib::ustring>& MidiReceiveChannelSplitSetting::enumToString() const
{
  static auto ret = StringTools::convert(getAllStrings<MidiReceiveChannelSplit>());
  return ret;
}

const std::vector<Glib::ustring>& MidiReceiveChannelSplitSetting::enumToDisplayString() const
{
  static std::vector<Glib::ustring> ret { "None", "Omni", "1",  "2",  "3",  "4",  "5",  "6",  "7",     "8",
                                          "9",    "10",   "11", "12", "13", "14", "15", "16", "Follow" };
  return ret;
}

MidiSendChannelSetting::MidiSendChannelSetting(UpdateDocumentContributor& s)
    : EnumSetting<MidiSendChannel>(s, MidiSendChannel::None)
{
}

const std::vector<Glib::ustring>& MidiSendChannelSetting::enumToString() const
{
  static auto ret = StringTools::convert(getAllStrings<MidiSendChannel>());
  return ret;
}

const std::vector<Glib::ustring>& MidiSendChannelSetting::enumToDisplayString() const
{
  static std::vector<Glib::ustring> ret { "None", "1",  "2",  "3",  "4",  "5",  "6",  "7", "8",
                                          "9",    "10", "11", "12", "13", "14", "15", "16" };
  return ret;
}

MidiSendChannelSplitSetting::MidiSendChannelSplitSetting(UpdateDocumentContributor& s)
    : EnumSetting<MidiSendChannelSplit>(s, MidiSendChannelSplit::None)
{
}

const std::vector<Glib::ustring>& MidiSendChannelSplitSetting::enumToString() const
{
  static auto ret = StringTools::convert(getAllStrings<MidiSendChannelSplit>());
  return ret;
}

const std::vector<Glib::ustring>& MidiSendChannelSplitSetting::enumToDisplayString() const
{
  static std::vector<Glib::ustring> ret { "None", "1",  "2",  "3",  "4",  "5",  "6",  "7",  "8",
                                          "9",    "10", "11", "12", "13", "14", "15", "16", "Follow" };
  return ret;
}
