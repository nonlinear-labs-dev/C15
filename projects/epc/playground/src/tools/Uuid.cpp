#include "Uuid.h"
#include <uuid.h>

Uuid::Uuid()
{
  generate();
}

Uuid::Uuid(const Uuid &other)
    : id(other.id)
{
}

Uuid::Uuid(Uuid &&other) noexcept
    : id(std::move(other.id))
{
}

Uuid::Uuid(const std::string &id)
    : id(id)
{
}

Uuid::Uuid(const Glib::ustring &id)
    : id(id)
{
}

Uuid::Uuid(const char *id)
    : id(id)
{
}

Uuid::~Uuid()
{
}

Uuid &Uuid::operator=(const Uuid &other)
{
  id = other.id;
  return *this;
}

bool Uuid::operator==(const Uuid &other) const
{
  return other.id == id;
}

bool Uuid::operator!=(const Uuid &other) const
{
  return other.id != id;
}

bool Uuid::operator<(const Uuid &other) const
{
  return id < other.id;
}

const std::string &Uuid::raw() const
{
  return id;
}

std::string &Uuid::raw()
{
  return id;
}

bool Uuid::empty() const
{
  return id.empty();
}

Uuid Uuid::none()
{
  return Uuid("");
}

Uuid Uuid::init()
{
  return Uuid("Init");
}

Uuid Uuid::converted()
{
  return Uuid("Converted");
}

Uuid Uuid::restored()
{
  return Uuid("Restored");
}

void Uuid::generate()
{
  uuid_t g;
  uuid_generate(g);

  char txt[64];
  uuid_unparse(g, txt);
  id = txt;
}
