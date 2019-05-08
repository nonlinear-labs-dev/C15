#pragma once

#include "playground.h"

class LED
{
 public:
  LED();
  virtual ~LED();

  void setID(int id);
  int getID() const;

 private:
  int m_ledID;
};
