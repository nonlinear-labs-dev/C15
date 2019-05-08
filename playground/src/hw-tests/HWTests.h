#pragma once

#include "http/ContentSection.h"
#include "HWTest.h"
#include <set>

class NetworkRequest;
class Application;

class HWTests : public ContentSection
{
 public:
  HWTests(UpdateDocumentContributor *parent);
  virtual ~HWTests();

  Glib::ustring getPrefix() const override;
  void writeDocument(Writer &writer, tUpdateID knownRevision) const override;

 private:
  void handleHTTPRequest(shared_ptr<NetworkRequest> request, const Glib::ustring &path) override;
  void initialize();

  bool m_init;

  typedef shared_ptr<HWTest> tTest;
  set<tTest> m_availableTests;
};
