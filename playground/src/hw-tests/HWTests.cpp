#include "HWTests.h"
#include "playground.h"
#include "http/SoupOutStream.h"
#include <http/NetworkRequest.h>
#include "xml/XmlWriter.h"
#include "Options.h"
#include "Application.h"
#include <giomm/file.h>
#include "device-settings/DebugLevel.h"

HWTests::HWTests(UpdateDocumentContributor *parent)
    : ContentSection(parent)
{
  m_init = false;
}

HWTests::~HWTests()
{
  DebugLevel::warning(__PRETTY_FUNCTION__, __LINE__);
}

Glib::ustring HWTests::getPrefix() const
{
  return "hw_tests";
}

void HWTests::writeDocument(Writer &writer, tUpdateID knownRevision) const
{
}

void HWTests::handleHTTPRequest(shared_ptr<NetworkRequest> request, const Glib::ustring &path)
{
  initialize();

  Glib::ustring searchPath = path.substr(1);

  auto it = find_if(m_availableTests.begin(), m_availableTests.end(), [&](tTest test) -> bool {
    Glib::ustring link = getPrefix() + "/" + test->getFileName();
    return searchPath == link;
  });

  if(it != m_availableTests.end())
  {
    (*it)->execute();
  }

  auto stream = request->createStream("text/html", false);
  XmlWriter writer(stream);

  writer.writeTag("div", [&]() {
    for(tTest test : m_availableTests)
    {
      writer.writeTag("div", [&]() {
        writer.writeTextElement("span", test->getName());

        stringstream onclick;
        onclick << "$.get('" << '/' << getPrefix() << '/' << test->getFileName() << "');";

        if(test->getState() != HWTest::RUNNING)
        {
          writer.writeTextElement("span", "execute", Attribute("onclick", onclick.str()));
          writer.writeTextElement("span", "cancel");
        }
        else
        {
          writer.writeTextElement("span", "execute");
          writer.writeTextElement("span", "cancel", Attribute("onclick", onclick.str()));
        }

        writer.writeTextElement("div", test->getOutput());
      });
    }
  });
}

void HWTests::initialize()
{
  if(!m_init)
  {
    m_init = true;

    m_availableTests.clear();

    Glib::RefPtr<Gio::Cancellable> cancellable = Gio::Cancellable::create();

    RefPtr<Gio::File> testFiles = Gio::File::create_for_path(Application::get().getOptions()->getHardwareTestsFolder());
    RefPtr<Gio::FileEnumerator> childrenEnumerator = testFiles->enumerate_children(cancellable);

    while(RefPtr<Gio::FileInfo> fileInfo = childrenEnumerator->next_file())
    {
      shared_ptr<HWTest> test(new HWTest(fileInfo));

      test->onTestChanged([=]() {

      });

      m_availableTests.insert(test);
    }
  }
}
