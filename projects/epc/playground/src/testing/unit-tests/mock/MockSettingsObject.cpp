#include "MockSettingsObject.h"

const Glib::ustring& MockSettingsObject::getSettingFileNameToLoadFrom() const
{
  static Glib::ustring emptyXMLPath = "";
  return emptyXMLPath;
}

UpdateDocumentContributor::tUpdateID MockSettingsObject::onChange(uint64_t flags)
{
  if(m_onChangeCB)
    return m_onChangeCB(flags);

  return Settings::onChange(flags);
}

void MockSettingsObject::setOnChangeCB(MockSettingsObject::tOnChangeCB&& cb)
{
  m_onChangeCB = cb;
}
