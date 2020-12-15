#pragma once
#include <ParameterId.h>
#include <playground.h>

class EditBuffer;
class ModulateableParameter;

class ModParameterUseCases
{
 public:
  explicit ModParameterUseCases(ModulateableParameter* param);

  void recallMCPos();
  void recallMCSource();
  void recallMCAmount();

  void undoRecallMCSel(MacroControls& controls);
  void undoRecallMCAmt(tControlPositionValue mcAmount);
  void undoRecallMCPos(tControlPositionValue mcPos);

  void incMCSelection(int incs);
  void incModAmount(int incs, bool fine);

  void selectModSource(MacroControls mc);

 private:
  ModulateableParameter* m_modParam = nullptr;
};
