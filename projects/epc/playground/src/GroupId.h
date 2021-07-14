#pragma once

#include <nltools/Types.h>

class GroupId
{
 public:
  GroupId(std::string name, VoiceGroup group);
  GroupId(const GroupId &other);

  explicit GroupId(const std::string &str);
  explicit GroupId(const Glib::ustring &str);

  bool operator<(const GroupId &other) const;
  bool operator==(const GroupId &other) const;
  bool operator!=(const GroupId &other) const;

  [[nodiscard]] const std::string &getName() const;
  [[nodiscard]] VoiceGroup getVoiceGroup() const;

  [[nodiscard]] std::string toString() const
  {
    return ::toString(getVoiceGroup()) + "-" + getName();
  }

  static bool isGlobal(const std::string &name);

 private:
  std::string m_name;
  VoiceGroup m_group;
};

inline std::ostream &operator<<(std::ostream &stream, const GroupId &e)
{
  stream << e.toString();
  return stream;
}
