#include "FuxieSwarmsTestBank.h"

FuxieSwarmsTestBank::FuxieSwarmsTestBank()
{
  const auto bankFile = getSourceDir() + "/projects/playground/test-resources/Fuxi_Swarms.xml";
  auto scope = TestHelper::createTestScope();
  auto transaction = scope->getTransaction();
  auto eb = TestHelper::getEditBuffer();
  auto pm = eb->getParent();

  m_bank = [&] {
    FileInStream stream(bankFile, false);
    auto useCase = pm->findActionManager<BankActions>();
    return useCase.importBank(transaction, stream, "0", "0", bankFile);
  }();
}

FuxieSwarmsTestBank::~FuxieSwarmsTestBank()
{
  const auto bankFile = getSourceDir() + "/projects/playground/test-resources/Fuxi_Swarms.xml";
  auto scope = TestHelper::createTestScope();
  auto transaction = scope->getTransaction();
  auto eb = TestHelper::getEditBuffer();
  auto pm = eb->getParent();

  pm->deleteBank(transaction, m_bank->getUuid());
}

Preset* FuxieSwarmsTestBank::getPreset(int index)
{
  return m_bank->getPresetAt(index);
}
