#pragma once

#include "playground.h"
#include <nltools/Types.h>
#include <functional>
#include <map>

class ParameterImportConversions
{
 public:
  static ParameterImportConversions &get();

  typedef std::function<tControlPositionValue(tControlPositionValue, SoundType)> tConverter;
  typedef int tFileVersion;
  typedef uint16_t tParameterID;

  [[nodiscard]] tControlPositionValue convert(tParameterID parameterID, tControlPositionValue in,
                                              tFileVersion inVersion, SoundType type) const;
  [[nodiscard]] tControlPositionValue convertMCAmount(tParameterID parameterID, tControlPositionValue in,
                                                      tFileVersion inVersion) const;

  virtual ~ParameterImportConversions();

 private:
  explicit ParameterImportConversions(bool registerDefaults);

  void registerConverter(tParameterID parameterID, tFileVersion srcVersion, tConverter c);
  void registerMCAmountConverter(tParameterID parameterID, tFileVersion srcVersion, tConverter c);

  [[nodiscard]] tControlPositionValue attackV2ToV3(tControlPositionValue in) const;
  [[nodiscard]] tControlPositionValue decayV2ToV3(tControlPositionValue in) const;
  [[nodiscard]] tControlPositionValue releaseV2ToV3(tControlPositionValue in) const;
  [[nodiscard]] tControlPositionValue driveV2ToV3(tControlPositionValue in) const;
  [[nodiscard]] tControlPositionValue driveV5ToV6(tControlPositionValue in) const;
  [[nodiscard]] tControlPositionValue voicesV5ToV6(tControlPositionValue unisonVoices) const;
  [[nodiscard]] tControlPositionValue voicesV7ToV8(tControlPositionValue unisonVoices, SoundType type) const;
  [[nodiscard]] tControlPositionValue splitV8ToV9(tControlPositionValue split) const;

  struct ConvertersBySourceFileVersion
  {
    [[nodiscard]] tControlPositionValue convert(tControlPositionValue in, tFileVersion inVersion, SoundType type) const;
    std::map<tFileVersion, tConverter> from;
  };

  std::map<tParameterID, ConvertersBySourceFileVersion> m_converters;
  std::map<tParameterID, ConvertersBySourceFileVersion> m_mcAmountConverters;

  friend class TestableImportConversions;
};
