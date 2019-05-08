#include "TextEditControl.h"
#include "TextEditControlLetter.h"
#include <proxies/hwui/TextEditUsageMode.h>

const int c_letterWidth = 8;

TextEditControl::TextEditControl(const Rect &pos, shared_ptr<TextEditUsageMode> textUsageMode)
    : super(pos)
{
  int numLetters = pos.getWidth() / c_letterWidth;

  if(numLetters % 2)
    numLetters--;

  int x = (pos.getWidth() - (numLetters * c_letterWidth)) / 2;
  int centerIdx = numLetters / 2;

  for(int i = 0; i < numLetters; i++)
  {
    addControl(new TextEditControlLetter(textUsageMode, i - centerIdx,
                                         Rect(x + i * c_letterWidth, 0, c_letterWidth, pos.getHeight())));
  }
}
