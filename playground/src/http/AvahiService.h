#pragma once

#include "playground.h"

#include <avahi-client/publish.h>
#include <avahi-glib/glib-watch.h>

class AvahiService
{
 public:
  AvahiService();
  virtual ~AvahiService();

 private:
  void registerAvahiServer();

  static void entry_group_callback(AvahiEntryGroup *g, AvahiEntryGroupState state, AvahiService *pThis);
  static void avahiClientCallback(AvahiClient *c, AvahiClientState state, AvahiService *pThis);

  Glib::ustring m_avahiServiceName;
  AvahiClient *m_avahiClient = nullptr;
  AvahiEntryGroup *m_avahiServiceGroup = nullptr;
  AvahiGLibPoll *m_ahaviGlibPoller = nullptr;
};
