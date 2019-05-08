#include <proxies/hwui/panel-unit/boled/undo/PrevUndoTransactionSiblingControl.h>
#include <proxies/hwui/Oleds.h>

PrevUndoTransactionSiblingControl::PrevUndoTransactionSiblingControl(const Rect &r)
    : super(r)
{
}

PrevUndoTransactionSiblingControl::~PrevUndoTransactionSiblingControl()
{
}

void PrevUndoTransactionSiblingControl::assignTransaction(shared_ptr<UNDO::Transaction> transaction, bool selected,
                                                          bool current)
{
  super::assignTransaction(transaction, selected, current);

  if(transaction)
  {
    if(auto parent = transaction->getPredecessor())
    {
      if(parent->getSuccessor(0) != transaction)
      {
        setText("<");
        setBold(transaction->isDefaultRouteLeft());
        return;
      }
    }
  }

  setText("");
}

void PrevUndoTransactionSiblingControl::setBold(bool bold)
{
  if(bold != m_bold)
  {
    m_bold = bold;
    setDirty();
  }
}

shared_ptr<Font> PrevUndoTransactionSiblingControl::getFont() const
{
  if(m_bold)
    return Oleds::get().getFont("Emphase_9_Bold", getFontHeight());

  return Oleds::get().getFont("Emphase_9_Regular", getFontHeight());
}

void PrevUndoTransactionSiblingControl::setFontColor(FrameBuffer &fb) const
{
  fb.setColor(FrameBuffer::Colors::C255);
}
