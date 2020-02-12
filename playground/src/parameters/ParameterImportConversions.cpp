#include <parameters/ParameterImportConversions.h>
#include <testing/TestDriver.h>

static TestDriver<ParameterImportConversions> tests;

ParameterImportConversions& ParameterImportConversions::get()
{
  static ParameterImportConversions conversions(true);
  return conversions;
}

ParameterImportConversions::ParameterImportConversions(bool registerDefaults)
{
  if(registerDefaults)
  {
    registerConverter(71, 2, [=](tControlPositionValue v) { return driveV2ToV3(v); });

    registerConverter(101, 2, [=](tControlPositionValue v) { return driveV2ToV3(v); });

    registerConverter(181, 2, [=](tControlPositionValue v) { return driveV2ToV3(v); });

    registerConverter(164, 3, [=](tControlPositionValue v) { return driveV2ToV3(v); });

    registerMCAmountConverter(71, 2, [=](tControlPositionValue v) { return driveV2ToV3(v); });

    registerMCAmountConverter(101, 2, [=](tControlPositionValue v) { return driveV2ToV3(v); });

    registerMCAmountConverter(181, 2, [=](tControlPositionValue v) { return driveV2ToV3(v); });

    registerMCAmountConverter(164, 3, [=](tControlPositionValue v) { return driveV2ToV3(v); });

    registerConverter(0, 2, [=](tControlPositionValue v) { return attackV2ToV3(v); });

    registerConverter(19, 2, [=](tControlPositionValue v) { return attackV2ToV3(v); });

    registerConverter(38, 2, [=](tControlPositionValue v) { return attackV2ToV3(v); });

    registerConverter(2, 2, [=](tControlPositionValue v) { return decayV2ToV3(v); });

    registerConverter(6, 2, [=](tControlPositionValue v) { return decayV2ToV3(v); });

    registerConverter(21, 2, [=](tControlPositionValue v) { return decayV2ToV3(v); });

    registerConverter(25, 2, [=](tControlPositionValue v) { return decayV2ToV3(v); });

    registerConverter(40, 2, [=](tControlPositionValue v) { return decayV2ToV3(v); });

    registerConverter(44, 2, [=](tControlPositionValue v) { return decayV2ToV3(v); });

    registerConverter(10, 2, [=](tControlPositionValue v) { return releaseV2ToV3(v); });

    registerConverter(29, 2, [=](tControlPositionValue v) { return releaseV2ToV3(v); });

    registerConverter(46, 2, [=](tControlPositionValue v) { return releaseV2ToV3(v); });

    registerConverter(127, 4, [=](tControlPositionValue v) {
      if(v > 0.995)
        return 1.0;

      return 0.5 + v / 1.99;
    });

    registerMCAmountConverter(127, 4, [=](tControlPositionValue v) { return v / 2.0; });

    registerConverter(238, 4, [=](tControlPositionValue v) {
      if(v > 0.5)
        return v;

      return v * v * v * 4.0;
    });

    registerConverter(240, 4, [=](tControlPositionValue v) { return 0.25 + v * 0.75; });

    registerConverter(249, 5, [=](tControlPositionValue v) { return voicesV5ToV6(v); });

    registerConverter(164, 5, [=](tControlPositionValue v) { return driveV5ToV6(v); });
  }
}

ParameterImportConversions::~ParameterImportConversions() = default;

tControlPositionValue ParameterImportConversions::attackV2ToV3(tControlPositionValue in) const
{
  if(in < 0.01)
    return 0.01 / 1.13 + (0.01 - 0.01 / 1.13) * (in / 0.01);

  return in;
}

tControlPositionValue ParameterImportConversions::decayV2ToV3(tControlPositionValue in) const
{
  if(in > 0.8078)
    return 1.0;

  return in + 0.1922;
}

tControlPositionValue ParameterImportConversions::releaseV2ToV3(tControlPositionValue in) const
{
  if(in > 0.8078)
    return 100.0 / 101;

  return (in + 0.1922) * 100.0 / 101;
}

tControlPositionValue ParameterImportConversions::driveV2ToV3(tControlPositionValue in) const
{
  // 0 ...25 -> 0 .. 50
  return in / 2;
}

tControlPositionValue ParameterImportConversions::driveV5ToV6(tControlPositionValue in) const
{
  // 0 .. 50 -> 0 .. 70
  return std::min((5.0 / 7.0) * in + (2.0 / 7.0), 1.0);
}

void ParameterImportConversions::registerConverter(const tParameterID parameterID, const tFileVersion srcVersion,
                                                   tConverter c)
{
  m_converters[parameterID].from[srcVersion] = c;
}

void ParameterImportConversions::registerMCAmountConverter(const tParameterID parameterID,
                                                           const tFileVersion srcVersion, tConverter c)
{
  m_mcAmountConverters[parameterID].from[srcVersion] = c;
}

tControlPositionValue ParameterImportConversions::convert(const tParameterID parameterID,
                                                          const tControlPositionValue in,
                                                          const tFileVersion inVersion) const
{
  auto it = m_converters.find(parameterID);

  if(it != m_converters.end())
  {
    return it->second.convert(in, inVersion);
  }

  return in;
}

tControlPositionValue ParameterImportConversions::convertMCAmount(const tParameterID parameterID,
                                                                  const tControlPositionValue in,
                                                                  const tFileVersion inVersion) const
{
  auto it = m_mcAmountConverters.find(parameterID);

  if(it != m_mcAmountConverters.end())
  {
    return it->second.convert(in, inVersion);
  }

  return in;
}

tControlPositionValue
    ParameterImportConversions::ConvertersBySourceFileVersion::convert(const tControlPositionValue in,
                                                                       const tFileVersion inVersion) const
{
  auto it = from.lower_bound(inVersion);

  if(it != from.end())
  {
    const tControlPositionValue convertedValue = it->second(in);
    return convert(convertedValue, it->first + 1);
  }

  return in;
}

void ParameterImportConversions::registerTests()
{
  g_test_add_func("/ParameterImportConversions/a", []() {
    ParameterImportConversions a(false);

    a.registerConverter(10, 4, [](tControlPositionValue v) { return v * 2; });

    a.registerConverter(10, 7, [](tControlPositionValue v) { return v * 4; });

    g_assert(a.convert(9, 0.5, 2) == 0.5);
    g_assert(a.convert(9, 0.5, 4) == 0.5);
    g_assert(a.convert(9, 0.5, 7) == 0.5);

    g_assert(a.convert(10, 0.5, 0) == 0.5 * 2 * 4);
    g_assert(a.convert(10, 0.5, 1) == 0.5 * 2 * 4);
    g_assert(a.convert(10, 0.5, 2) == 0.5 * 2 * 4);
    g_assert(a.convert(10, 0.5, 3) == 0.5 * 2 * 4);
    g_assert(a.convert(10, 0.5, 4) == 0.5 * 2 * 4);
    g_assert(a.convert(10, 0.5, 5) == 0.5 * 4);
    g_assert(a.convert(10, 0.5, 6) == 0.5 * 4);
    g_assert(a.convert(10, 0.5, 7) == 0.5 * 4);
    g_assert(a.convert(10, 0.5, 8) == 0.5);
    g_assert(a.convert(10, 0.5, 9) == 0.5);
  });
}

tControlPositionValue ParameterImportConversions::voicesV5ToV6(tControlPositionValue unisonVoices) const
{
  if(unisonVoices == 1.0)
    return 0.5 - (1.0 / 24.0);
  return unisonVoices * 0.5;
}
