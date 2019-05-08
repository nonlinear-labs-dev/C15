#include <proxies/hwui/panel-unit/boled/undo/NextUndoTransactionSiblingControl.h>
#include <proxies/hwui/Oleds.h>

NextUndoTransactionSiblingControl::NextUndoTransactionSiblingControl(const Rect &r)
    : super(r)
{
}

NextUndoTransactionSiblingControl::~NextUndoTransactionSiblingControl()
{
}

void NextUndoTransactionSiblingControl::assignTransaction(shared_ptr<UNDO::Transaction> transaction, bool selected,
                                                          bool current)
{
  super::assignTransaction(transaction, selected, current);

  if(transaction)
  {
    if(auto parent = transaction->getPredecessor())
    {
      size_t numChildren = parent->getNumSuccessors();

      if(parent->getSuccessor(numChildren - 1) != transaction)
      {
        setText(">");
        setBold(transaction->isDefaultRouteRight());
        return;
      }
    }
  }

  setText("");
}

void NextUndoTransactionSiblingControl::setBold(bool bold)
{
  if(bold != m_bold)
  {
    m_bold = bold;
    setDirty();
  }
}

shared_ptr<Font> NextUndoTransactionSiblingControl::getFont() const
{
  if(m_bold)
    return Oleds::get().getFont("Emphase_9_Bold", getFontHeight());

  return Oleds::get().getFont("Emphase_9_Regular", getFontHeight());
}

void NextUndoTransactionSiblingControl::setFontColor(FrameBuffer &fb) const
{
  fb.setColor(FrameBuffer::Colors::C255);
}
