#pragma once

#include <nltools/Types.h>
#include <ParameterId.h>
#include <sigc++/connection.h>
#include "ParameterCarousel.h"

class SpecialParameterCarousel : public ParameterCarousel
{
 public:
  explicit SpecialParameterCarousel(const Rect &rect);
  ~SpecialParameterCarousel() override;
  void setup(Parameter *selectedParameter) override;

 private:
  virtual std::vector<ParameterId> getParameterIdsForMode(SoundType type) = 0;
  void setupControls(Parameter *parameter);
  void rebuild();

  sigc::connection m_editbufferConnection;
  sigc::connection m_voiceGroupConnection;
};
