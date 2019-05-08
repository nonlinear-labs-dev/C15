#pragma once

class Hysteresis
{
 public:
  Hysteresis(double initVal, double hyst);

  double applyChange(double v);
  double set(double v);

  static void registerTests();

 private:
  double m_value;
  double m_hyst;
  double m_rangeMin;
  double m_rangeMax;
  double m_reportedValue;
};
