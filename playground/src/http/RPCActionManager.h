#pragma once

#include "playground.h"
#include <libsoup/soup.h>
#include <functional>
#include "http/NetworkRequest.h"

class RPCActionManager
{
 public:
  RPCActionManager(const Glib::ustring &basePath);
  virtual ~RPCActionManager();

  bool matches(const Glib::ustring &path) const;
  virtual bool handleRequest(const Glib::ustring &path, shared_ptr<NetworkRequest> request);
  bool handleRequest(shared_ptr<NetworkRequest> request);
  const Glib::ustring &getBasePath() const;

  typedef function<void(shared_ptr<NetworkRequest> request)> tAction;
  void addAction(const Glib::ustring &path, tAction action);

 private:
  Glib::ustring m_basePath;
  map<Glib::ustring, tAction> m_actions;
};
