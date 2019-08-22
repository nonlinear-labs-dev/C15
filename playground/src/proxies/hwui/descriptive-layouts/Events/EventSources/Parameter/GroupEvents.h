#pragma once

namespace DescriptiveLayouts
{
  class CurrentParameterGroupLockStatus : public ParameterEvent<bool>
  {
   public:
    explicit CurrentParameterGroupLockStatus()
    {
      m_locksConnection = Application::get().getPresetManager()->getEditBuffer()->onLocksChanged(
          sigc::mem_fun(this, &CurrentParameterGroupLockStatus::onLockChanged));
    }

    ~CurrentParameterGroupLockStatus()
    {
      m_locksConnection.disconnect();
    }

    void onSelectedParameterChanged(const Parameter *p) override
    {
      setValue(p ? p->isLocked() : false);
    }

   private:
    void onLockChanged()
    {
      onSelectedParameterChanged(Application::get().getPresetManager()->getEditBuffer()->getSelected());
    }

   protected:
    sigc::connection m_locksConnection;
  };
}