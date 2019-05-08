#include "NetworkRequest.h"

NetworkRequest::NetworkRequest()
{
}

NetworkRequest::~NetworkRequest()
{
}

Glib::ustring NetworkRequest::get(const Glib::ustring &key, Glib::ustring def) const
{
  auto it = m_keyValues.find(key);

  if(it != m_keyValues.end())
    return it->second;

  return def;
}

bool NetworkRequest::isOracle() const
{
  return get("isOracle", "0") == "1";
}

void NetworkRequest::addKeyValuePairToMap(const char *key, const char *value, tKeyValues *map)
{
  map->insert(make_pair(key, value));
}

void NetworkRequest::forEach(std::function<void(const Glib::ustring &key, const Glib::ustring &val)> cb)
{
  for(auto &it : m_keyValues)
  {
    cb(it.first, it.second);
  }
}

void NetworkRequest::createMapFromQueryString(const Glib::ustring &queryString)
{
  GHashTable *q = soup_form_decode(queryString.c_str());
  g_hash_table_foreach(q, (GHFunc) &NetworkRequest::addKeyValuePairToMap, (gpointer) &m_keyValues);
  g_hash_table_unref(q);
}
