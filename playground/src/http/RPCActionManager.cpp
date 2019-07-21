#include "RPCActionManager.h"
#include "device-settings/DebugLevel.h"

RPCActionManager::RPCActionManager(const Glib::ustring &basePath)
    : m_basePath(basePath)
{
}

RPCActionManager::~RPCActionManager()
{
}

const Glib::ustring &RPCActionManager::getBasePath() const
{
  return m_basePath;
}

void RPCActionManager::addAction(const Glib::ustring &path, tAction action)
{
  g_assert(m_actions.find(path) == m_actions.end());
  m_actions[path] = action;
}

bool RPCActionManager::matches(const Glib::ustring &path) const
{
  return path.find(m_basePath) == 0;
}

bool RPCActionManager::handleRequest(const Glib::ustring &path, std::shared_ptr<NetworkRequest> request)
{
  const auto &it = m_actions.find(path.substr(m_basePath.length()));

  if(it != m_actions.end())
  {
    DebugLevel::gassy("doing action", path);
    it->second(request);
    request->okAndComplete();
    return true;
  }

  return false;
}

bool RPCActionManager::handleRequest(std::shared_ptr<NetworkRequest> request)
{
  return handleRequest(request->getPath(), request);
}
