#pragma once
#include <ParameterId.h>
#include <playground.h>

class EditBuffer;
class Parameter;

class ParameterUseCases
{
 public:
  explicit ParameterUseCases(Parameter* parameter);

  void recallParameterFromPreset();
  void undoRecallParameterFromPreset(tControlPositionValue cp);

  void incDec(int incs, bool fine, bool shift);
  void setDefault();
  
 private:
  Parameter* m_parameter = nullptr;
};