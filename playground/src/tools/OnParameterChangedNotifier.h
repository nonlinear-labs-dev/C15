#pragma once

#include <Application.h>
#include <parameters/Parameter.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>

template <class T> class OnParameterChangedNotifier
{
 public:
  explicit OnParameterChangedNotifier(T *parent)
  {
    m_parent = parent;
    Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
        sigc::mem_fun(this, &OnParameterChangedNotifier::onParameterSelectionChanged));
  }

  void onParameterSelectionChanged(const Parameter *o, Parameter *n)
  {
    m_connection.disconnect();
    if(n)
    {
      m_connection = n->onParameterChanged(sigc::mem_fun(m_parent, &T::onParameterChanged), true);
    }
  }

 protected:
  T *m_parent;
  sigc::connection m_connection;
};

template <class T> class OnParameterSelectionChangedNotifier
{
 public:
  explicit OnParameterSelectionChangedNotifier(T *parent)
  {
    m_parent = parent;
    Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
        sigc::mem_fun(m_parent, &T::onParameterSelectionChanged));
  }

 protected:
  T *m_parent;
  sigc::connection m_connection;
};