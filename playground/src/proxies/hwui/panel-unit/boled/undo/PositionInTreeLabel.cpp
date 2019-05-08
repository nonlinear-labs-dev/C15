#include <proxies/hwui/panel-unit/boled/undo/PositionInTreeLabel.h>

PositionInTreeLabel::PositionInTreeLabel(const Rect &pos)
    : super(pos)
{
}

PositionInTreeLabel::~PositionInTreeLabel()
{
}

void PositionInTreeLabel::assignTransaction(shared_ptr<UNDO::Transaction> transaction, bool selected, bool current)
{
  int above = transaction->countPredecessors();
  int below = transaction->countSuccessorsOnDefaultRoute();
  setText(to_string(above + 1) + "/" + to_string(above + below + 1));
}
