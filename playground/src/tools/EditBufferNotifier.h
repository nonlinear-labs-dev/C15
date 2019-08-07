#pragma once

#include <sigc++/trackable.h>
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>

template <class T> class OnEditBufferChangedNotifier : public sigc::trackable
{
 public:
  explicit OnEditBufferChangedNotifier(T *parent)
  {
    m_parent = parent;

    m_connection = Application::get().getPresetManager()->getEditBuffer()->onChange(
        sigc::mem_fun(this, &OnEditBufferChangedNotifier::onEditBufferChanged));
  }

  ~OnEditBufferChangedNotifier()
  {
    m_connection.disconnect();
  }

  void onEditBufferChanged()
  {
    if(m_parent)
      m_parent->onEditBufferChanged(Application::get().getPresetManager()->getEditBuffer());
  }

 protected:
  T *m_parent;
  sigc::connection m_connection;
};
