#pragma once

#include "playground.h"
#include "parameters/ValueRange.h"

class QuantizedValue;

class ScaleConverter
{
 public:
  typedef ValueRange<tTcdValue> tTcdRange;
  typedef ValueRange<tDisplayValue> tDisplayRange;
  typedef ValueRange<tControlPositionValue> tControlPositionRange;

  virtual ~ScaleConverter();

  virtual tControlPositionValue getCoarseDenominator(const QuantizedValue &v) const;
  virtual tControlPositionValue getFineDenominator(const QuantizedValue &v) const;

  virtual tDisplayValue controlPositionToDisplay(const tControlPositionValue &cpValue) const = 0;
  virtual tTcdValue controlPositionToTcd(const tControlPositionValue &cpValue) const = 0;
  virtual tControlPositionValue tcdToControlPosition(tTcdValue v) const = 0;

  virtual Glib::ustring controlPositionToDisplayJS() const = 0;

  const ScaleConverter::tControlPositionRange getControlPositionRange() const;
  virtual bool isBiPolar() const = 0;

  const Dimension &getDimension() const;
  tTcdRange getTcdRange() const;

  template <typename T> static const ScaleConverter *get()
  {
    static map<int, unique_ptr<ScaleConverter>> s_converters;

    auto key = typeid(T).hash_code();
    auto it = s_converters.find(key);

    if(it == s_converters.end())
    {
      auto ret = new T();
      s_converters[key] = unique_ptr<T>(ret);
      return ret;
    }
    return it->second.get();
  }

  virtual size_t hash() const;

  static const tControlPositionRange &getControlPositionRangeUnipolar();
  static const tControlPositionRange &getControlPositionRangeBipolar();

 protected:
  ScaleConverter(const Dimension &dim);

 private:
  const Dimension &m_dimension;
};
