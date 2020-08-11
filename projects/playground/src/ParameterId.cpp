#include "ParameterId.h"
#include <utility>
#include <parameter_list.h>
#include <glibmm/ustring.h>

static int parseNumber(const std::string &in)
{
  auto p = in.find('-');

  if(p == std::string::npos)
    return std::atoi(in.data());

  return std::atoi(in.data() + p + 1);
}

static VoiceGroup parseGroup(const std::string &in)
{
  auto p = in.find('-');

  if(p == std::string::npos)
  {
    auto n = parseNumber(in);
    return ParameterId::isGlobal(n) ? VoiceGroup::Global : VoiceGroup::I;
  }

  return to<VoiceGroup>(in.substr(0, p));
}

ParameterId::ParameterId(int num, VoiceGroup group)
    : m_num(num)
    , m_group(group)
{
}

ParameterId::ParameterId(const ParameterId &other)
    : m_num(other.m_num)
    , m_group(other.m_group)
{
}

ParameterId::ParameterId(const std::string &str)
    : m_num(parseNumber(str))
    , m_group(parseGroup(str))
{
}

ParameterId::ParameterId(const Glib::ustring &str)
    : ParameterId(str.raw())
{
}

bool ParameterId::operator<(const ParameterId &other) const
{
  return std::tie(m_num, m_group) < std::tie(other.m_num, other.m_group);
}

bool ParameterId::operator==(const ParameterId &other) const
{
  return std::tie(m_num, m_group) == std::tie(other.m_num, other.m_group);
}

bool ParameterId::operator!=(const ParameterId &other) const
{
  return std::tie(m_num, m_group) != std::tie(other.m_num, other.m_group);
}

uint16_t ParameterId::getNumber() const
{
  return m_num;
}

VoiceGroup ParameterId::getVoiceGroup() const
{
  return m_group;
}

bool ParameterId::isDual() const
{
  return isDual(getNumber());
}

bool ParameterId::isGlobal() const
{
  return isGlobal(getNumber());
}

bool ParameterId::isGlobal(int number)
{
  if(number >= 400 || number < 0)
    return false;

  auto &paramDescript = C15::ParameterList[number];
  auto type = paramDescript.m_param.m_type;
  return type != C15::Descriptors::ParameterType::Local_Modulateable
      && type != C15::Descriptors::ParameterType::Local_Unmodulateable;
}

ParameterId ParameterId::invalid()
{
  return ParameterId { std::numeric_limits<decltype(m_num)>::max(), VoiceGroup::NumGroups };
}

bool ParameterId::isDual(int number)
{
  return !isGlobal(number);
}
