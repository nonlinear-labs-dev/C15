#pragma once

class PresetManager;
class Clipboard;

//TODO Populate

class ClipBoardUseCases
{
 public:
  ClipBoardUseCases(Clipboard* cb, PresetManager* pm);

 private:
  Clipboard* m_clipBoard;
  PresetManager* m_presetManager;
};
