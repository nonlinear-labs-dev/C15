#include "XmlReader.h"
#include "serialization/Serializer.h"
#include <unordered_set>
#include "../device-settings/DebugLevel.h"

XmlReader::XmlReader(InStream& in, UNDO::Transaction* transaction)
    : super(in, transaction)
    , m_context(*this)
{
}

XmlReader::~XmlReader()
{
}

void XmlReader::on_start_element(ParseContext& context, const Glib::ustring& element_name,
                                 const AttributeMap& attributes)
{
  std::hash<Glib::ustring> hash;

  Attributes cp;

  for(const auto& it : attributes)
    cp.set(it.first, it.second);

  if(!onStartElement(hash(element_name), cp))
  {
    m_lastAttributes = cp;
  }

  m_lastTag = element_name;
}

void XmlReader::on_text(ParseContext& context, const Glib::ustring& text)
{
  m_lastText = text;
  m_lastTag = context.get_element();
}

void XmlReader::on_end_element(ParseContext& context, const Glib::ustring& element_name)
{
  if(element_name == m_lastTag)
  {
    try
    {
      std::hash<Glib::ustring> hash;
      onTextElement(hash(m_lastTag), m_lastAttributes, trim(m_lastText));
    }
    catch(...)
    {
      DebugLevel::debug("XmlReader::on_end_element, element_name: ", element_name);
    }
  }

  onEndElement();
}

Glib::ustring XmlReader::trim(const Glib::ustring& text)
{
  static std::unordered_set<std::string> emptyStrings;

  if(emptyStrings.find(text.raw()) != emptyStrings.end())
    return Glib::ustring();

  auto begin = text.end();
  auto end = text.end();

  if(text.begin() != text.end())
  {
    if(g_unichar_isgraph(*(text.begin())) && g_unichar_isgraph(*(text.rbegin())))
    {
      return text;
    }
  }

  for(auto a = text.begin(); a != text.end(); a++)
  {
    if(g_unichar_isgraph(*a))
    {
      if(begin == text.end())
        begin = a;

      end = a;
    }
  }

  if(end != text.end())
    end++;

  auto ret = Glib::ustring(begin, end);

  if(ret.empty())
    emptyStrings.insert(text.raw());

  return ret;
}

void XmlReader::feed()
{
  do
  {
    m_context.parse(getInStream().read());
  } while(!getInStream().eof());
}
