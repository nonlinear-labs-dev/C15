#include <parameters/names/RowStream.h>

RowStream::RowStream(const std::string &fileName)
    : m_stream(fileName)
{
}

RowStream::~RowStream()
{
}

bool RowStream::eatRow()
{
  std::string waste;
  return pop(waste);
}

void RowStream::forEach(function<void(const std::string &)> cb)
{
  std::string row;

  while(pop(row))
    cb(row);
}

bool RowStream::pop(std::string &line)
{
  line.clear();
  std::string buffer;

  bool insideQuote = false;

  while(getline(m_stream, buffer))
  {
    insideQuote = iterateQuotes(buffer, insideQuote);
    line.append(buffer);

    if(!insideQuote)
      break;

    line.append("\n");
  }

  bool gotStuff = !line.empty();
  bool stuffToExpect = m_stream.good() && !m_stream.eof();

  return gotStuff || stuffToExpect;
}

bool RowStream::iterateQuotes(const std::string &buffer, bool insideQuote) const
{
  auto lastQuote = buffer.find_first_of('"');
  while(lastQuote != string::npos)
  {
    insideQuote = !insideQuote;
    lastQuote = buffer.find_first_of('"', lastQuote + 1);
  }
  return insideQuote;
}
