#pragma once

#include "playground.h"
#include <string>

class Uuid
{
 public:
  Uuid();
  Uuid(const Uuid &other);
  Uuid(Uuid &&other) noexcept;

  explicit Uuid(const std::string &id);
  explicit Uuid(const Glib::ustring &id);
  explicit Uuid(const char *id);

  virtual ~Uuid();

  void generate();
  Uuid &operator=(const Uuid &other);
  bool operator==(const Uuid &other) const;
  bool operator!=(const Uuid &other) const;
  bool operator<(const Uuid &other) const;

  [[nodiscard]] const std::string &raw() const;
  std::string &raw();
  [[nodiscard]] bool empty() const;

  static Uuid none();
  static Uuid init();
  static Uuid converted();
  static Uuid restored();

 private:
  std::string id;
};
