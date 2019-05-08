#pragma once

#include "playground.h"

class ParameterDB
{
 public:
  static ParameterDB &get();

  virtual ~ParameterDB();

  ustring getLongName(int id) const;
  ustring getShortName(int id) const;
  tControlPositionValue getSignalPathIndication(int id) const;

  static constexpr tControlPositionValue getInvalidSignalPathIndication()
  {
    return std::numeric_limits<tControlPositionValue>::max();
  }

 private:
  ParameterDB();

  void read();
  void parseCSVRow(const std::string &row);
  vector<std::string> textRowToVector(const std::string &row) const;
  void importParsedRow(vector<std::string> &&items);
  tControlPositionValue parseSignalPathIndication(const std::string &c) const;

  struct ParameterSpec
  {
    ustring longName;
    ustring shortName;
    tControlPositionValue signalPathIndication;
  };

  std::map<int, ParameterSpec> m_spec;
};
