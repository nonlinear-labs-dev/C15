#pragma once

#include <nltools/Types.h>
#include <glibmm/ustring.h>

class EditBuffer;
class PresetManager;

class SoundUseCases
{
 public:
  SoundUseCases(EditBuffer* eb, PresetManager* pm);

  void storeInitSound();
  void resetInitSound();

  void initPart(VoiceGroup part);
  void initSound();

  void randomizeSound(double amount);
  void randomizePart(VoiceGroup part, double randomizeAmount);

  void renamePart(VoiceGroup part, const Glib::ustring& name);

  void convertToSingle(VoiceGroup partToUse);
  void convertToLayer(VoiceGroup currentPart);
  void convertToSplit(VoiceGroup currentPart);

 private:
  EditBuffer* m_editBuffer;
  PresetManager* m_presetManager;
};