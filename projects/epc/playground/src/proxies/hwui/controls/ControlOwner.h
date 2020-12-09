#pragma once

#include "playground.h"
#include "Rect.h"
#include <nltools/Uncopyable.h>
#include <list>
#include <memory>
#include <functional>

class Control;
class FrameBuffer;

class ControlOwner : public Uncopyable
{
 public:
  ControlOwner();
  virtual ~ControlOwner();

  virtual bool redraw(FrameBuffer &fb);
  virtual void setHighlight(bool isHighlight);

  virtual bool isHighlight() const;
  virtual void setDirty() = 0;

  typedef std::shared_ptr<Control> tControlPtr;
  typedef std::list<tControlPtr> tControls;

  const tControls &getControls() const;

  template <class T> std::list<std::shared_ptr<T>> getControls() const
  {
    std::list<std::shared_ptr<T>> ret {};
    for(auto &c : m_controls)
    {
      if(auto casted = std::dynamic_pointer_cast<T>(c))
      {
        ret.emplace_back(casted);
      }
    }
    return ret;
  }

  template <typename T> void highlight()
  {
    for(auto &c : getControls())
      if(std::dynamic_pointer_cast<T>(c))
        highlight(c);
  }

  template <typename T> void lowlight()
  {
    for(auto &c : getControls())
      if(auto cc = std::dynamic_pointer_cast<T>(c))
        cc->setHighlight(false);
  }

  template <typename T> std::shared_ptr<T> findControlOfType()
  {
    for(auto &c : getControls())
      if(auto p = std::dynamic_pointer_cast<T>(c))
        return p;

    return nullptr;
  }

  template <typename T> const std::shared_ptr<T> findControlOfType() const
  {
    for(auto &c : getControls())
      if(auto p = std::dynamic_pointer_cast<T>(c))
        return p;

    return nullptr;
  }

  template <typename... TControls> struct ControlList
  {
    template <typename CB> static bool findFirstAndCall(ControlOwner *o, CB cb)
    {
      using this_type = ControlList<TControls...>;
      bool found = false;
      std::initializer_list<bool> { (this_type::callIfMatches<CB, TControls>(found, o, cb), false)... };
      return found;
    }

    template <typename CB, typename T> static void callIfMatches(bool &found, ControlOwner *c, CB cb)
    {
      if(!found)
      {
        if(auto a = c->findControlOfType<T>())
        {
          found = true;
          cb(a.get());
        }
      }
    }
  };

  void highlight(const std::shared_ptr<Control> &c);
  void noHighlight();
  void highlightButtonWithCaption(const Glib::ustring &caption);
  void highlightButtonWithCaption(const Glib::ustring &caption, bool desiredHighlight);

  template <typename T> T *addControl(T *ctrl)
  {
    if(ctrl == nullptr)
      return nullptr;

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
  void forEach(const tIfCallback &cb) const;

  typedef std::function<void(tControlPtr)> tCallback;
  void forEach(const tCallback &cb) const;

  tControlPtr first();

 private:
  tControls m_controls;
};
