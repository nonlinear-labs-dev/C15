#pragma once

#include <http/ContentSection.h>
#include <list>
#include <memory>
#include <http/RPCActionManager.h>

class DeviceInformationItem;

class DeviceInformation : public ContentSection
{
 public:
  DeviceInformation(UpdateDocumentContributor *parent);
  virtual ~DeviceInformation();

  virtual void handleHTTPRequest(shared_ptr<NetworkRequest> request, const Glib::ustring &path) override;
  virtual void writeDocument(Writer &writer, tUpdateID knownRevision) const override;
  virtual Glib::ustring getPrefix() const override;

  Glib::ustring getSoftwareVersion() const;

  typedef shared_ptr<DeviceInformationItem> tItem;
  typedef std::list<tItem> tItemList;

  template <typename T> shared_ptr<T> getItem()
  {
    for(auto &s : m_items)
      if(shared_ptr<T> r = dynamic_pointer_cast<T>(s))
        return r;

    return nullptr;
  }

 private:
  tItemList m_items;
  RPCActionManager m_actions;
};
