#include <Application.h>
#include <playground.h>
#include <boost/tokenizer.hpp>
#include <parameters/names/ParameterDB.h>
#include "RowStream.h"

ParameterDB &ParameterDB::get()
{
  static ParameterDB db;
  return db;
}

ParameterDB::ParameterDB()
{
  read();
}

ParameterDB::~ParameterDB()
{
}

void ParameterDB::read()
{
  RowStream in(Application::get().getResourcePath() + "ParameterList.csv");
  in.eatRow();
  in.forEach(bind(&ParameterDB::parseCSVRow, this, std::placeholders::_1));
}

void ParameterDB::parseCSVRow(const std::string &row)
{
  try
  {
    importParsedRow(textRowToVector(row));
  }
  catch(...)
  {
  }
}

std::vector<std::string> ParameterDB::textRowToVector(const std::string &row) const
{
  typedef boost::escaped_list_separator<char> tSeparator;
  typedef boost::tokenizer<tSeparator> tTokenizer;
  tTokenizer tok(row, tSeparator('\\', ',', '\"'));
  return vector<std::string>(tok.begin(), tok.end());
}

void ParameterDB::importParsedRow(vector<std::string> &&items)
{
  int id = stoi(items[0]);
  m_spec[id] = { items[4], items[8], parseSignalPathIndication(items[10]) };
}

tControlPositionValue ParameterDB::parseSignalPathIndication(const std::string &c) const
{
  try
  {
    return stod(c);
  }
  catch(...)
  {
    return getInvalidSignalPathIndication();
  }
}

ustring ParameterDB::getLongName(int id) const
{
  return m_spec.at(id).longName;
}

ustring ParameterDB::getShortName(int id) const
{
  return m_spec.at(id).shortName;
}

tControlPositionValue ParameterDB::getSignalPathIndication(int id) const
{
  return m_spec.at(id).signalPathIndication;
}
