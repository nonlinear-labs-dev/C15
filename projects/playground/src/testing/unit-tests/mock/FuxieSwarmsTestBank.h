#pragma once
#include <testing/TestHelper.h>
#include <CompileTimeOptions.h>
#include <xml/FileInStream.h>
#include <presets/BankActions.h>
#include <presets/Bank.h>

class FuxieSwarmsTestBank
{
 public:
  FuxieSwarmsTestBank();

  ~FuxieSwarmsTestBank();

  Preset* getPreset(int index);

 private:
  Bank* m_bank;
};
