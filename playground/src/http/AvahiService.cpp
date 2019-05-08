#include "AvahiService.h"
#include <avahi-common/alternative.h>
#include <avahi-common/malloc.h>
#include "device-settings/DebugLevel.h"

AvahiService::AvahiService()
{
  return;

  m_avahiServiceName = "Nonlinear Labs";
  m_avahiClient = NULL;
  m_avahiServiceGroup = NULL;

  m_ahaviGlibPoller = avahi_glib_poll_new(NULL, G_PRIORITY_LOW);
  m_avahiClient = avahi_client_new(avahi_glib_poll_get(m_ahaviGlibPoller), (AvahiClientFlags) 0,
                                   (AvahiClientCallback) &AvahiService::avahiClientCallback, this, NULL);

  registerAvahiServer();
}

AvahiService::~AvahiService()
{
}

void AvahiService::entry_group_callback(AvahiEntryGroup *g, AvahiEntryGroupState state, AvahiService *pThis)
{
  switch(state)
  {
    case AVAHI_ENTRY_GROUP_ESTABLISHED:
      DebugLevel::info("avahi ready");
      break;

    case AVAHI_ENTRY_GROUP_COLLISION:
    {
      char *newName = avahi_alternative_service_name(pThis->m_avahiServiceName.c_str());
      pThis->m_avahiServiceName = newName;
      avahi_free(newName);
      pThis->registerAvahiServer();
      break;
    }

    case AVAHI_ENTRY_GROUP_FAILURE:
      break;

    case AVAHI_ENTRY_GROUP_UNCOMMITED:
      break;

    case AVAHI_ENTRY_GROUP_REGISTERING:
      break;
  }
}

void AvahiService::registerAvahiServer()
{
  AvahiIfIndex idx = AVAHI_IF_UNSPEC;

  avahi_entry_group_add_service(m_avahiServiceGroup, idx, AVAHI_PROTO_UNSPEC, (AvahiPublishFlags) 0,
                                m_avahiServiceName.c_str(), "_http._tcp", NULL, NULL, 8080, NULL);

  avahi_entry_group_commit(m_avahiServiceGroup);
}

void AvahiService::avahiClientCallback(AvahiClient *c, AvahiClientState state, AvahiService *pThis)
{
  switch(state)
  {
    case AVAHI_CLIENT_S_RUNNING:
      if(pThis->m_avahiServiceGroup == NULL)
      {
        pThis->m_avahiServiceGroup
            = avahi_entry_group_new(c, (AvahiEntryGroupCallback) &AvahiService::entry_group_callback, pThis);
        pThis->registerAvahiServer();
      }
      break;

    case AVAHI_CLIENT_S_COLLISION:
      return;

    case AVAHI_CLIENT_S_REGISTERING:
      if(pThis->m_avahiServiceGroup)
        avahi_entry_group_reset(pThis->m_avahiServiceGroup);

      pThis->m_avahiServiceGroup = NULL;
      break;

    default:
      break;
  }
}
