#pragma once

#include "ParameterCarousel.h"
#include <parameters/Parameter.h>

class VoiceGroupMasterParameterCarousel : public ParameterCarousel
{
 public:
  explicit VoiceGroupMasterParameterCarousel(const Rect &r);
  ~VoiceGroupMasterParameterCarousel() override;

 private:
  void rebuild();

 protected:
  void setup(Parameter *selectedParameter) override;
  void setupMasterParameters(const std::vector<ParameterId> &parameters);

 private:
  sigc::connection m_editbufferConnection;
  sigc::connection m_selectConnection;
};
