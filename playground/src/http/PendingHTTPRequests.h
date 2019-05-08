#pragma once

#include "playground.h"
#include <set>
#include "NetworkRequest.h"

class PendingHTTPRequests
{
 public:
  PendingHTTPRequests();
  virtual ~PendingHTTPRequests();

  typedef shared_ptr<NetworkRequest> tRequest;
  typedef set<tRequest> tMessageSet;
  void addPendingMessage(tRequest msg);
  tMessageSet expropriateFromPendingMessages();
  bool removeIfExists(tRequest msg);
  bool removeIfExists(SoupMessage *msg);

 private:
  tMessageSet m_pendingMessages;
};
