#include <proxies/hwui/panel-unit/boled/undo/UndoTransactionNameControl.h>

UndoTransactionNameControl::UndoTransactionNameControl(const Rect &r)
    : super(r)
{
}

UndoTransactionNameControl::~UndoTransactionNameControl()
{
}

void UndoTransactionNameControl::assignTransaction(shared_ptr<UNDO::Transaction> transaction, bool selected,
                                                   bool current)
{
  super::assignTransaction(transaction, selected, current);

  if(transaction)
    setText(transaction->getName());
  else
    setText("");
}

Glib::ustring UndoTransactionNameControl::shortenStringIfNeccessary(shared_ptr<Font> font,
                                                                    const Glib::ustring &text) const
{
  auto itLeft = text.begin();
  itLeft++;

  auto itRight = text.end();
  itRight--;
  itRight--;

  Glib::ustring lastResult;

  while(itLeft < itRight)
  {
    Glib::ustring result = Glib::ustring(text.begin(), itLeft) + "..." + Glib::ustring(itRight, text.end());

    int width = font->getStringWidth(result);

    if(width > getPosition().getWidth())
      break;

    lastResult = result;

    itLeft++;
    itRight--;
  }

  return lastResult;
}
