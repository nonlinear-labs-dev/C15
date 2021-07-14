#pragma once
class Preset;
class Bank;

class MockPreset
{
 public:
  MockPreset(Bank* parent);
  ~MockPreset();
  Preset* get();

 private:
  Bank* m_bank;
};

class PreDualModePresetBank
{
 public:
  static MockPreset createMockPreset();
  static const char* getMockData();
};