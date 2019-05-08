#include "Uuid.h"
#include <uuid.h>

Uuid::Uuid()
{
  generate();
}

Uuid::Uuid(const Uuid &other)
{
  id = other.id;
}

Uuid::Uuid(const Glib::ustring &id)
{
  this->id = id;
}

Uuid::Uuid(const std::string &id)
{
  this->id = id;
}

Uuid::~Uuid()
{
}

Uuid &Uuid::operator=(const Uuid &other)
{
  id = other.id;
  return *this;
}

Uuid::operator Glib::ustring() const
{
  return id;
}

bool Uuid::operator==(const Uuid &other) const
{
  return other.id == id;
}

bool Uuid::operator==(const Glib::ustring &other) const
{
  return id == other;
}

void Uuid::generate()
{
  uuid_t g;
  uuid_generate(g);

  char txt[64];
  uuid_unparse(g, txt);
  id = txt;
}
