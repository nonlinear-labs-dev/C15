#include "AlsaDeviceMonitor.h"
#include <giomm.h>
#include <alsa/asoundlib.h>
#include <nltools/StringTools.h>

AlsaDeviceMonitor::AlsaDeviceMonitor(Callback cb)
    : m_cb(cb)
{
  Glib::MainContext::get_default()->signal_timeout().connect_seconds(sigc::mem_fun(this, &AlsaDeviceMonitor::poll), 1);
}

void AlsaDeviceMonitor::setEnabled(bool e)
{
  m_enabled = e;
}

void collectPorts(AlsaDeviceMonitor::InputDevices &target, snd_ctl_t *ctl, snd_rawmidi_info_t *info, int card,
                  int device, snd_rawmidi_stream_t stream)
{
  snd_rawmidi_info_set_stream(info, stream);
  snd_ctl_rawmidi_info(ctl, info);

  for(auto sub = 0; sub < snd_rawmidi_info_get_subdevices_count(info); ++sub)
  {
    snd_rawmidi_info_set_stream(info, stream);
    snd_rawmidi_info_set_subdevice(info, sub);
    snd_ctl_rawmidi_info(ctl, info);
    auto sub_name = snd_rawmidi_info_get_subdevice_name(info);

    if(sub == 0 && sub_name[0] == '\0')
    {
      target.push_back(nltools::string::concat("hw:", card, ",", device));
      break;
    }
    else
    {
      target.push_back(nltools::string::concat("hw:", card, ",", device, ",", sub));
    }
  }
}

void recurseDevices(AlsaDeviceMonitor::InputDevices &ins, AlsaDeviceMonitor::OutputDevices &outs, snd_ctl_t *ctl,
                    int card, int device)
{
  if(snd_ctl_rawmidi_next_device(ctl, &device) == 0)
  {
    if(device >= 0)
    {
      snd_rawmidi_info_t *info;
      snd_rawmidi_info_alloca(&info);
      snd_rawmidi_info_set_device(info, device);
      collectPorts(ins, ctl, info, card, device, SND_RAWMIDI_STREAM_INPUT);
      collectPorts(outs, ctl, info, card, device, SND_RAWMIDI_STREAM_OUTPUT);
      recurseDevices(ins, outs, ctl, card, device);
    }
  }
}

void recurseCard(AlsaDeviceMonitor::InputDevices &ins, AlsaDeviceMonitor::OutputDevices &outs, int card)
{
  snd_ctl_t *ctl;
  char name[32];

  sprintf(name, "hw:%d", card);

  if(snd_ctl_open(&ctl, name, 0) == 0)
  {
    recurseDevices(ins, outs, ctl, card, -1);
    snd_ctl_close(ctl);
  }
}

void recurseCards(AlsaDeviceMonitor::InputDevices &ins, AlsaDeviceMonitor::OutputDevices &outs, int card)
{
  if(snd_card_next(&card) == 0)
  {
    if(card >= 0)
    {
      recurseCard(ins, outs, card);
      recurseCards(ins, outs, card);
    }
  }
}

bool AlsaDeviceMonitor::poll()
{
  InputDevices ins;
  OutputDevices outs;

  if(m_enabled)
    recurseCards(ins, outs, -1);

  if(m_ins != ins || m_outs != outs)
  {
    m_ins = ins;
    m_outs = outs;
    m_cb(ins, outs);
  }

  return true;
}
