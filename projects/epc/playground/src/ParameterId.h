#pragma once

#include <nltools/Types.h>

class ParameterId
{
 public:
  ParameterId(int num, VoiceGroup group);
  ParameterId(const ParameterId &other);
  explicit ParameterId(const std::string &str);
  explicit ParameterId(const Glib::ustring &str);

  bool operator<(const ParameterId &other) const;
  bool operator==(const ParameterId &other) const;
  bool operator!=(const ParameterId &other) const;

  [[nodiscard]] uint16_t getNumber() const;
  [[nodiscard]] VoiceGroup getVoiceGroup() const;
  [[nodiscard]] bool isDual() const;
  [[nodiscard]] bool isGlobal() const;

  [[nodiscard]] std::string toString() const
  {
    return ::toString(getVoiceGroup()) + "-" + std::to_string(getNumber());
  }

  friend std::ostream &operator<<(std::ostream &stream, const ParameterId &e)
  {
    stream << e.toString();
    return stream;
  }

  static bool isGlobal(int number);
  static bool isDual(int number);

  static ParameterId invalid();

 private:
  uint16_t m_num;
  VoiceGroup m_group;
};

namespace std
{
  template <> struct hash<ParameterId> : public __hash_base<size_t, ParameterId>
  {
    size_t operator()(const ParameterId &p) const noexcept
    {
      return (static_cast<size_t>(p.getNumber()) << 16) | static_cast<size_t>(p.getVoiceGroup());
    }
  };
}
