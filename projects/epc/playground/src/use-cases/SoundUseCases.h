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

  void randomizeSound();
  void randomizePart(VoiceGroup part);

  void renamePart(VoiceGroup part, const Glib::ustring& name);

  void convertToSingle(VoiceGroup partToUse);
  void convertToLayer();
  void convertToSplit();

 private:
  EditBuffer* m_editBuffer;
  PresetManager* m_presetManager;
};