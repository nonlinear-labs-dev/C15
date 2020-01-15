#include "EditBufferEvent.h"

DescriptiveLayouts::EventMerger &DescriptiveLayouts::getTheEventMerger(EditBuffer *eb, HWUI *hwui)
{
  static EventMerger theMerger(eb, hwui);
  return theMerger;
}
