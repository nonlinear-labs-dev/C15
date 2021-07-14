#pragma once
#include <string>

class C15Synth;

class XMLPresetLoader
{
 public:
  static void loadTestPreset(C15Synth *synth, const std::string &subDir, const std::string &uuid);
  static void loadTestPresetFromBank(C15Synth *synth, const std::string &subDir, const std::string &bankFileName);
};
