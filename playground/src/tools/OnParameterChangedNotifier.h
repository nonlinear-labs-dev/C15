#pragma once

#include <Application.h>
#include <parameters/Parameter.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <parameters/ModulateableParameter.h>
#include <parameters/MacroControlParameter.h>

template <class T> class OnParameterChangedNotifier : public sigc::trackable
{
 public:
  explicit OnParameterChangedNotifier(T *parent)
  {
    m_parent = parent;
    Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
        sigc::mem_fun(this, &OnParameterChangedNotifier::onParameterSelectionChanged));
  }

  ~OnParameterChangedNotifier()
  {
    m_connection.disconnect();
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

template <class T> class OnParameterSelectionChangedNotifier : public sigc::trackable
{
 public:
  explicit OnParameterSelectionChangedNotifier(T *parent)
  {
    m_parent = parent;
    m_connection = Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
        sigc::mem_fun(m_parent, &T::onParameterSelectionChanged));
  }
  ~OnParameterSelectionChangedNotifier()
  {
    m_connection.disconnect();
  }

 protected:
  T *m_parent;
  sigc::connection m_connection;
};

template <class T> class OnModulationChangedNotifier : public sigc::trackable
{
 public:
  explicit OnModulationChangedNotifier(T *parent)
      : m_parent(parent)
  {
    m_onSelectionChangedSignal = Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
        sigc::mem_fun(this, &OnModulationChangedNotifier::onParameterSelectionChanged));
  }

  ~OnModulationChangedNotifier()
  {
    m_onModulationSourceChangedSignal.disconnect();
    m_onSelectionChangedSignal.disconnect();
    m_onParameterChangedSignal.disconnect();
  }

  void onParameterSelectionChanged(const Parameter *o, Parameter *n)
  {
    m_onModulationSourceChangedSignal.disconnect();
    m_onParameterChangedSignal.disconnect();

    if(n)
    {
      m_onParameterChangedSignal
          = n->onParameterChanged(sigc::mem_fun(this, &OnModulationChangedNotifier::onParameterChanged), true);

      if(auto modP = dynamic_cast<const ModulateableParameter *>(n))
      {
        if(auto mc = modP->getMacroControl())
        {
          m_onModulationSourceChangedSignal
              = mc->onParameterChanged(sigc::mem_fun(this, &OnModulationChangedNotifier::onParameterChanged), true);
        }
      }
    }
  }

  void onParameterChanged(const Parameter *param)
  {
    if(auto modP = dynamic_cast<const ModulateableParameter *>(param))
    {
      if(m_parent)
      {
        m_parent->onModulationSourceChanged(modP);
      }
    }
    else if(auto mc = dynamic_cast<const MacroControlParameter *>(param))
    {
      if(!mc->getTargets().empty())
      {
        if(m_parent)
        {
          for(const auto &target : mc->getTargets())
            m_parent->onModulationSourceChanged(target);
        }
      }
    }
  }

  void disconnect()
  {
    m_onModulationSourceChangedSignal.disconnect();
    m_onSelectionChangedSignal.disconnect();
    m_onParameterChangedSignal.disconnect();
    m_parent = nullptr;
  }

 protected:
  T *m_parent;
  sigc::connection m_onParameterChangedSignal;
  sigc::connection m_onSelectionChangedSignal;
  sigc::connection m_onModulationSourceChangedSignal;
};