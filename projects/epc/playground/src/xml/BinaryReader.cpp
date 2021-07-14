#include <xml/BinaryReader.h>

BinaryReader::BinaryReader(InStream &in, UNDO::Transaction *transaction)
    : Reader(in, transaction)
{
}

BinaryReader::~BinaryReader()
{
}

void BinaryReader::feed()
{
  m_buffer = getInStream().readAll();
  m_it = m_buffer.begin();
  parse();
}

void BinaryReader::parse()
{
  while(m_it != m_buffer.end())
  {
    switch(*(m_it++))
    {
      case 'X':
        readTextElement();
        break;

      case 'T':
        readTagElement();
        parse();
        break;

      case 't':
        onEndElement();
        return;

      case 'A':
        readAttribute();
        break;

      default:
        g_warn_if_reached();
    }
  }
}

void BinaryReader::readTextElement()
{
  auto name = readRawString();
  auto text = readRawString();
  auto attributes = readRawAttributes();

  std::hash<Glib::ustring> hash;
  onTextElement(hash(name), attributes, text);
}

void BinaryReader::readTagElement()
{
  auto name = readRawString();
  auto attributes = readRawAttributes();

  std::hash<Glib::ustring> hash;
  onStartElement(hash(name), attributes);
}

Attributes BinaryReader::readRawAttributes()
{
  Attributes ret;
  uint32_t numAttributes = readUInt();

  for(uint32_t i = 0; i < numAttributes; i++)
  {
    auto tag = *(m_it++);
    g_assert(tag == 'A');
    auto a = readAttribute();
    ret.set(a.getName(), a.getValue());
  }

  return ret;
}

Attribute BinaryReader::readAttribute()
{
  auto name = readRawString();
  auto text = readRawString();
  return { name, text };
}

uint32_t BinaryReader::readUInt()
{
  union
  {
    uint32_t ret = 0;
    uint8_t bytes[4];
  } r;

  r.bytes[0] = *(m_it++);
  r.bytes[1] = *(m_it++);
  r.bytes[2] = *(m_it++);
  r.bytes[3] = *(m_it++);

  return r.ret;
}

Glib::ustring BinaryReader::readRawString()
{
  auto numBytes = readUInt();
  std::string str(m_it, m_it + numBytes);
  g_assert(str.length() == numBytes);
  std::advance(m_it, numBytes);
  return str;
}
