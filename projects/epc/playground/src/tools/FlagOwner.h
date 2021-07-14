#pragma once

template <typename TFlags, typename tStorage = uint16_t> class FlagOwner
{
 public:
  FlagOwner() = default;
  virtual ~FlagOwner() = default;

  void setFlag(TFlags flag)
  {
    m_flags |= flag;
  }

  void removeFlag(TFlags flag)
  {
    tStorage u = flag;
    m_flags &= ~u;
  }

  bool isFlagSet(TFlags flag)
  {
    return (m_flags & flag) != 0;
  }

 private:
  tStorage m_flags = 0;
};
