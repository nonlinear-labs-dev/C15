#pragma once

#include "playground.h"
#include "parameters/ValueRange.h"
#include <map>
#include <memory>

class QuantizedValue;

class ScaleConverter
{
 public:
  typedef ValueRange<tTcdValue> tTcdRange;
  typedef ValueRange<tDisplayValue> tDisplayRange;
  typedef ValueRange<tControlPositionValue> tControlPositionRange;

  virtual ~ScaleConverter();

  [[nodiscard]] virtual tControlPositionValue getCoarseDenominator(const QuantizedValue &v) const;
  [[nodiscard]] virtual tControlPositionValue getFineDenominator(const QuantizedValue &v) const;

  [[nodiscard]] virtual tDisplayValue controlPositionToDisplay(const tControlPositionValue &cpValue) const = 0;
  [[nodiscard]] virtual tTcdValue controlPositionToTcd(const tControlPositionValue &cpValue) const = 0;
  [[nodiscard]] virtual tControlPositionValue tcdToControlPosition(tTcdValue v) const = 0;

  [[nodiscard]] virtual Glib::ustring controlPositionToDisplayJS() const = 0;

  [[nodiscard]] const ScaleConverter::tControlPositionRange getControlPositionRange() const;
  [[nodiscard]] virtual bool isBiPolar() const = 0;

  [[nodiscard]] const Dimension &getDimension() const;
  [[nodiscard]] tTcdRange getTcdRange() const;

  template <typename T> static const ScaleConverter *get()
  {
    static std::map<int, std::unique_ptr<ScaleConverter>> s_converters;

    auto key = typeid(T).hash_code();
    auto it = s_converters.find(key);

    if(it == s_converters.end())
    {
      s_converters[key] = std::make_unique<T>();
      return s_converters[key].get();
    }
    return it->second.get();
  }

  [[nodiscard]] virtual size_t hash() const;

  static const tControlPositionRange &getControlPositionRangeUnipolar();
  static const tControlPositionRange &getControlPositionRangeBipolar();

 protected:
  explicit ScaleConverter(const Dimension &dim);

 private:
  const Dimension &m_dimension;
};
