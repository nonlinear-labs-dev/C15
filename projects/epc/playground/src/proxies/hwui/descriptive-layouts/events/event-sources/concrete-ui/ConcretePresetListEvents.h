#pragma once

#include "../base/EventSource.h"
#include <presets/PresetManager.h>

namespace DescriptiveLayouts::PresetListEvents
{

  template <typename T> class PresetListBase : public EventSource<T>
  {
   public:
    PresetListBase() = default;

    ~PresetListBase() override
    {
      m_presetListConnection.disconnect();
    }

    virtual void onPresetListChanged(GenericPresetList *pl) = 0;

   protected:
    void onLayoutChanged(Layout *layout)
    {
      m_presetListConnection.disconnect();

      if(auto genericLayout = dynamic_cast<GenericLayout *>(layout))
      {
        if(auto presetlist = genericLayout->findControlOfType<GenericPresetList>())
        {
          m_presetListConnection = presetlist->onChange(sigc::mem_fun(this, &PresetListBase::onPresetListChanged));
        }
      }
    }

    sigc::connection m_presetListConnection;
  };

  class PresetListHasBankLeft : public PresetListBase<bool>
  {
   protected:
    void onPresetListChanged(GenericPresetList *pl) override
    {
      if(auto selection = pl->getPresetAtSelected())
      {
        if(auto bank = dynamic_cast<Bank *>(selection->getParent()))
        {
          auto pm = Application::get().getPresetManager();
          auto next = pm->getBankAt(pm->getBankPosition(bank->getUuid()) - 1);
          setValue(next != nullptr);
        }
      }
    }
  };

  class PresetListHasBankRight : public PresetListBase<bool>
  {
   protected:
    void onPresetListChanged(GenericPresetList *pl) override
    {
      if(auto selection = pl->getPresetAtSelected())
      {
        if(auto bank = dynamic_cast<Bank *>(selection->getParent()))
        {
          auto pm = Application::get().getPresetManager();
          auto next = pm->getBankAt(pm->getBankPosition(bank->getUuid()) + 1);
          setValue(next != nullptr);
        }
      }
    }
  };

  class PresetListBankName : public PresetListBase<DisplayString>
  {
   public:
    PresetListBankName()
    {
      setValue({ "No Bank", 0 });
    }

   protected:
    void onPresetListChanged(GenericPresetList *pl) override
    {
      if(pl == nullptr)
        return;

      if(auto selection = pl->getPresetAtSelected())
      {
        if(auto bank = dynamic_cast<Bank *>(selection->getParent()))
        {
          setValue({ bank->getName(true), 0 });
        }
      }
    }
  };

  class PresetListPresetName : public PresetListBase<DisplayString>
  {
   public:
    PresetListPresetName()
    {
      setValue({ "No Preset", 0 });
    }

   protected:
    void onPresetListChanged(GenericPresetList *pl) override
    {
      if(auto selection = pl->getPresetAtSelected())
      {
        setValue({ selection->getName(), 0 });
      }
    }
  };
}
