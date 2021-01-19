#include "MidiChannelSettings.h"

MidiReceiveChannelSetting::MidiReceiveChannelSetting(UpdateDocumentContributor& s)
    : NLEnumSetting<MidiReceiveChannel>(s, MidiReceiveChannel::None)
{
}

MidiReceiveChannelSplitSetting::MidiReceiveChannelSplitSetting(UpdateDocumentContributor& s)
    : NLEnumSetting<MidiReceiveChannelSplit>(s, MidiReceiveChannelSplit::None)
{
}

MidiSendChannelSetting::MidiSendChannelSetting(UpdateDocumentContributor& s)
    : NLEnumSetting<MidiSendChannel>(s, MidiSendChannel::None)
{
}

MidiSendChannelSplitSetting::MidiSendChannelSplitSetting(UpdateDocumentContributor& s)
    : NLEnumSetting<MidiSendChannelSplit>(s, MidiSendChannelSplit::None)
{
}
