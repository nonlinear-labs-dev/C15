#pragma once

#include "playground.h"

class Uuid
{
 public:
  Uuid();
  Uuid(const Uuid &other);
  Uuid(const Glib::ustring &id);
  Uuid(const std::string &id);
  virtual ~Uuid();

  void generate();
  Uuid &operator=(const Uuid &other);
  bool operator==(const Uuid &other) const;
  bool operator==(const Glib::ustring &other) const;

  operator Glib::ustring() const;

 private:
  Glib::ustring id;
};
