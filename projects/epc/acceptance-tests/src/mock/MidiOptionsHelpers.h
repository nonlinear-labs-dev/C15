#pragma once
#include <nltools/messaging/Message.h>

namespace MidiOptionsHelper
{
  typedef nltools::msg::Setting::MidiSettingsMessage tMSG;

  void configureOptions(MidiRuntimeOptions* options, const std::function<void(tMSG&)>& msgEditCB)
  {
    if(options)
    {
      tMSG msg {};
      msgEditCB(msg);
      options->update(msg);
    }
  };
}
