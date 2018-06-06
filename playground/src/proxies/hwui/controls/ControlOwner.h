#pragma once

#include "playground.h"
#include "Rect.h"
#include <tools/Uncopyable.h>
#include <list>

class Control;
class FrameBuffer;

class ControlOwner : public Uncopyable
{
  public:
    ControlOwner ();
    virtual ~ControlOwner ();

    void collectDirtyRects(std::list<Rect> &rects) const;
    void setDirtyIfOverlapsWithAny(const std::list<Rect> &rects);

    virtual bool isDirty() const;
    virtual bool redraw(FrameBuffer &fb);
    virtual void setHighlight(bool isHighlight);

    virtual bool isHighlight() const;
    virtual void setDirty() = 0;

    void setAllDirty();

    typedef shared_ptr<Control> tControlPtr;
    typedef list<tControlPtr> tControls;

    const tControls &getControls () const;

    template<typename T> void highlight ()
    {
      for (auto c : getControls())
        if(dynamic_pointer_cast<T>(c))
          highlight(c);
    }

    template<typename T> shared_ptr<T> findControlOfType ()
    {
      for (auto c : getControls())
        if(auto p = dynamic_pointer_cast<T>(c))
          return p;

      return nullptr;
    }

    void highlight(shared_ptr<Control> c);
    void noHighlight();
    void highlightButtonWithCaption (const Glib::ustring &caption);

  protected:
    template<typename T>
    T *addControl (T *ctrl)
    {
      m_controls.push_back (tControlPtr (ctrl));

      if(this->isHighlight())
        ctrl->setHighlight(true);

      this->setDirty();
      return ctrl;
    }

    void remove (const Control *ctrl);

    virtual void clear();
    size_t getNumChildren() const;

    typedef function<bool (tControlPtr)> tIfCallback;
    void forEach(tIfCallback cb) const;

    typedef function<void (tControlPtr)> tCallback;
    void forEach(tCallback cb) const;

    tControlPtr first();

  private:
    tControls m_controls;
};

