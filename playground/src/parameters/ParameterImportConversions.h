#pragma once

#include "playground.h"
#include <functional>
#include <map>

class ParameterImportConversions
{
 public:
  static ParameterImportConversions &get();

  typedef std::function<tControlPositionValue(tControlPositionValue)> tConverter;
  typedef int tFileVersion;
  typedef uint16_t tParameterID;

  tControlPositionValue convert(const tParameterID parameterID, const tControlPositionValue in,
                                const tFileVersion inVersion) const;
  tControlPositionValue convertMCAmount(const tParameterID parameterID, const tControlPositionValue in,
                                        const tFileVersion inVersion) const;

  virtual ~ParameterImportConversions();

  static void registerTests();

 private:
  ParameterImportConversions(bool registerDefaults);

  void registerConverter(const tParameterID parameterID, const tFileVersion srcVersion, tConverter c);
  void registerMCAmountConverter(const tParameterID parameterID, const tFileVersion srcVersion, tConverter c);

  tControlPositionValue attackV2ToV3(tControlPositionValue in) const;
  tControlPositionValue decayV2ToV3(tControlPositionValue in) const;
  tControlPositionValue releaseV2ToV3(tControlPositionValue in) const;
  tControlPositionValue driveV2ToV3(tControlPositionValue in) const;
  tControlPositionValue driveV5ToV6(tControlPositionValue in) const;
  tControlPositionValue voicesV5ToV6(tControlPositionValue unisonVoices) const;

  struct ConvertersBySourceFileVersion
  {
    tControlPositionValue convert(const tControlPositionValue in, const tFileVersion inVersion) const;
    std::map<tFileVersion, tConverter> from;
  };

  std::map<tParameterID, ConvertersBySourceFileVersion> m_converters;
  std::map<tParameterID, ConvertersBySourceFileVersion> m_mcAmountConverters;

};
