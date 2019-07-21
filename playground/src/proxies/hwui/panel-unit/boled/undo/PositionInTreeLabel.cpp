#include <proxies/hwui/panel-unit/boled/undo/PositionInTreeLabel.h>
#include <assert.h>

PositionInTreeLabel::PositionInTreeLabel(const Rect &pos)
    : super(pos)
{
}

PositionInTreeLabel::~PositionInTreeLabel() = default;

void PositionInTreeLabel::assignTransaction(UNDO::Transaction *transaction, bool selected, bool current)
{
  if(transaction)
  {
    int above = transaction->countPredecessors();
    int below = transaction->countSuccessorsOnDefaultRoute();
    setText({ to_string(above + 1) + "/" + to_string(above + below + 1) });
  }
}
