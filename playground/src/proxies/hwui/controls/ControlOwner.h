#pragma once

#include "playground.h"
#include "Rect.h"
#include <nltools/Uncopyable.h>
#include <list>

class Control;
class FrameBuffer;

class ControlOwner : public Uncopyable
{
 public:
  ControlOwner();
  virtual ~ControlOwner();

  virtual bool isDirty() const;
  virtual bool redraw(FrameBuffer &fb);
  virtual void setHighlight(bool isHighlight);

  virtual bool isHighlight() const;
  virtual void setDirty() = 0;

  void setAllDirty();

  typedef std::shared_ptr<Control> tControlPtr;
  typedef std::list<tControlPtr> tControls;

  const tControls &getControls() const;

  template <typename T> void highlight()
  {
    for(auto c : getControls())
      if(std::dynamic_pointer_cast<T>(c))
        highlight(c);
  }

  template <typename T> void lowlight()
  {
    for(auto c : getControls())
      if(auto cc = std::dynamic_pointer_cast<T>(c))
        cc->setHighlight(false);
  }

  template <typename T> std::shared_ptr<T> findControlOfType()
  {
    for(auto c : getControls())
      if(auto p = std::dynamic_pointer_cast<T>(c))
        return p;

    return nullptr;
  }

  void highlight(std::shared_ptr<Control> c);
  void noHighlight();
  void highlightButtonWithCaption(const Glib::ustring &caption);

  template <typename T> T *addControl(T *ctrl)
  {
    m_controls.push_back(tControlPtr(ctrl));

    if(this->isHighlight())
      ctrl->setHighlight(true);

    this->setDirty();
    return ctrl;
  }


 protected:
  void remove(const Control *ctrl);

  virtual void clear();
  size_t getNumChildren() const;

  typedef std::function<bool(tControlPtr)> tIfCallback;
  void forEach(tIfCallback cb) const;

  typedef std::function<void(tControlPtr)> tCallback;
  void forEach(tCallback cb) const;

  tControlPtr first();

 private:
  tControls m_controls;
};
