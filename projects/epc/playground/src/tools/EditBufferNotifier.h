#pragma once

#include <sigc++/trackable.h>
#include <sigc++/connection.h>

class EditBuffer;

class OnEditBufferChangedNotifierBase : public sigc::trackable
{
 public:
  explicit OnEditBufferChangedNotifierBase();
  virtual ~OnEditBufferChangedNotifierBase() = default;
  virtual void onEditBufferChanged() = 0;

 protected:
  EditBuffer *getEditBuffer();
  sigc::connection m_connection;
};

template <class T> class OnEditBufferChangedNotifier : public OnEditBufferChangedNotifierBase
{
 public:
  explicit OnEditBufferChangedNotifier(T *parent)
      : m_parent(parent)
  {
  }

  void onEditBufferChanged() override
  {
    if(m_parent)
      m_parent->onEditBufferChanged(getEditBuffer());
  }

 protected:
  T *m_parent;
};
