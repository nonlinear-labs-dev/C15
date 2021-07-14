#include <testing/TestHelper.h>
#include <parameters/RibbonParameter.h>
#include <parameters/ModulationRoutingParameter.h>
#include <parameter_declarations.h>
#include <use-cases/RibbonParameterUseCases.h>
#include <use-cases/PhysicalControlParameterUseCases.h>

TEST_CASE("100% HW-Amount of Bipolar Source on MC used to lead to 2x the modulation to happen")
{
  auto eb = TestHelper::getEditBuffer();

  ParameterId srcID = { C15::PID::Ribbon_1, VoiceGroup::Global };
  auto ribbon1 = eb->findAndCastParameterByID<RibbonParameter>(srcID);
  auto mcA = eb->findAndCastParameterByID<MacroControlParameter>({ C15::PID::MC_A, VoiceGroup::Global });
  auto connectionParam
      = eb->findAndCastParameterByID<ModulationRoutingParameter>({ C15::PID::Ribbon_1_to_MC_A, VoiceGroup::Global });

  //INIT
  {
    auto scope = TestHelper::createTestScope();
    TestHelper::initSingleEditBuffer(scope->getTransaction());
  }

  RibbonParameterUseCases ribbonUseCase(ribbon1);
  ribbonUseCase.setReturnMode(RibbonReturnMode::RETURN);

  MacroControlParameterUseCases mcUseCase(mcA);
  mcUseCase.setControlPosition(0.5);

  ParameterUseCases routeUseCase(connectionParam);
  routeUseCase.setControlPosition(1);

  WHEN("Ribbon untouched")
  {

    THEN("Mod Checks out")
    {
      TestHelper::doMainLoopIteration();
      CHECK(ribbon1->getControlPositionValue() == 0);
      CHECK(mcA->getControlPositionValue() == 0.5);
    }
  }

  WHEN("Ribbon is -1")
  {
    ribbonUseCase.setControlPosition(-1.);

    THEN("Mod Checks out")
    {
      TestHelper::doMainLoopIteration();
      CHECK(ribbon1->getControlPositionValue() == -1);
      CHECK(mcA->getControlPositionValue() == 0);
    }
  }

  WHEN("Ribbon is 1")
  {
    ribbonUseCase.setControlPosition(1.);

    THEN("Mod Checks out")
    {
      TestHelper::doMainLoopIteration();
      CHECK(ribbon1->getControlPositionValue() == 1);
      CHECK(mcA->getControlPositionValue() == 1);
    }
  }

  //This documents the feature that mapped HWs with 100% Amt can bring an MC into clipping
  WHEN("Ribbon is -0.5")
  {
    ribbonUseCase.setControlPosition(-0.5);

    THEN("Mod Checks out")
    {
      TestHelper::doMainLoopIteration();
      CHECK(ribbon1->getControlPositionValue() == -0.5);
      CHECK(mcA->getControlPositionValue() == 0);
    }
  }

  WHEN("Ribbon is 0.5")
  {
    ribbonUseCase.setControlPosition(.5);

    THEN("Mod Checks out")
    {
      TestHelper::doMainLoopIteration();
      CHECK(ribbon1->getControlPositionValue() == .5);
      CHECK(mcA->getControlPositionValue() == 1);
    }
  }
}