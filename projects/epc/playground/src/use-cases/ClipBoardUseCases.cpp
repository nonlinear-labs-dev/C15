#include "ClipBoardUseCases.h"
#include <clipboard/Clipboard.h>

ClipBoardUseCases::ClipBoardUseCases(Clipboard* cb, PresetManager* pm)
    : m_presetManager { pm }
    , m_clipBoard { cb }
{
}
