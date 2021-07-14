#include <proxies/hwui/panel-unit/boled/undo/UndoTransactionNameControl.h>
#include <libundo/undo/Transaction.h>

UndoTransactionNameControl::UndoTransactionNameControl(const Rect &r)
    : super(r)
{
}

UndoTransactionNameControl::~UndoTransactionNameControl()
{
}

void UndoTransactionNameControl::assignTransaction(UNDO::Transaction *transaction, bool selected, bool current)
{
  super::assignTransaction(transaction, selected, current);

  if(transaction)
    setText(transaction->getName());
  else
    setText("");
}

StringAndSuffix UndoTransactionNameControl::shortenStringIfNeccessary(const std::shared_ptr<Font> &font,
                                                                      const StringAndSuffix &text) const
{
  auto itLeft = text.text.begin();
  itLeft++;

  auto itRight = text.text.end();
  itRight--;
  itRight--;

  Glib::ustring lastResult;

  while(itLeft < itRight)
  {
    Glib::ustring result = Glib::ustring(text.text.begin(), itLeft) + "..." + Glib::ustring(itRight, text.text.end());

    int width = font->getStringWidth(result);

    if(width > getPosition().getWidth())
      break;

    lastResult = result;

    itLeft++;
    itRight--;
  }

  return lastResult;
}
