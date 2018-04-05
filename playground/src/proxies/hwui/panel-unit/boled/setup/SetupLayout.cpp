#include <Application.h>
#include <device-info/DateTimeInfo.h>
#include <device-info/DeviceInformation.h>
#include <device-info/OSVersion.h>
#include <device-info/RTSoftwareVersion.h>
#include <device-info/SoftwareVersion.h>
#include <device-settings/PedalType.h>
#include <groups/HardwareSourcesGroup.h>
#include <parameters/PedalParameter.h>
#include <presets/EditBuffer.h>
#include <presets/PresetManager.h>
#include <proxies/hwui/FrameBuffer.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/Oleds.h>
#include <proxies/hwui/panel-unit/boled/BOLED.h>
#include <proxies/hwui/panel-unit/boled/setup/AboutLayout.h>
#include <proxies/hwui/panel-unit/boled/setup/AftertouchEditor.h>
#include <proxies/hwui/panel-unit/boled/setup/AftertouchView.h>
#include <proxies/hwui/panel-unit/boled/setup/BenderCurveEditor.h>
#include <proxies/hwui/panel-unit/boled/setup/BenderCurveView.h>
#include <proxies/hwui/panel-unit/boled/setup/DeviceInfoItemView.h>
#include <proxies/hwui/panel-unit/boled/setup/DeviceNameView.h>
#include <proxies/hwui/panel-unit/boled/setup/DateTimeEditor.h>
#include <proxies/hwui/panel-unit/boled/setup/EditSmoothingTimeEditor.h>
#include <proxies/hwui/panel-unit/boled/setup/EditSmoothingTimeView.h>
#include <proxies/hwui/panel-unit/boled/setup/EncoderAccelerationEditor.h>
#include <proxies/hwui/panel-unit/boled/setup/EncoderAccelerationView.h>
#include <proxies/hwui/panel-unit/boled/setup/ExportBackupEditor.h>
#include <proxies/hwui/panel-unit/boled/setup/FreeInternalMemoryView.h>
#include <proxies/hwui/panel-unit/boled/setup/PassphraseEditor.h>
#include <proxies/hwui/panel-unit/boled/setup/PassphraseView.h>
#include <proxies/hwui/panel-unit/boled/setup/PedalEditor.h>
#include <proxies/hwui/panel-unit/boled/setup/PedalSelectionControl.h>
#include <proxies/hwui/panel-unit/boled/setup/PedalView.h>
#include <proxies/hwui/panel-unit/boled/setup/PresetGlitchSuppressionEditor.h>
#include <proxies/hwui/panel-unit/boled/setup/PresetGlitchSuppressionView.h>
#include <proxies/hwui/panel-unit/boled/setup/RenameDeviceLayout.h>
#include <proxies/hwui/panel-unit/boled/setup/RibbonRelativeFactorSettingEditor.h>
#include <proxies/hwui/panel-unit/boled/setup/RibbonRelativeFactorSettingView.h>
#include <proxies/hwui/panel-unit/boled/setup/SetupEditor.h>
#include <proxies/hwui/panel-unit/boled/setup/SetupLayout.h>
#include <proxies/hwui/panel-unit/boled/setup/SetupSelectionLabel.h>
#include <proxies/hwui/panel-unit/boled/setup/SSIDView.h>
#include <proxies/hwui/panel-unit/boled/setup/UpdateAvailableView.h>
#include <proxies/hwui/panel-unit/boled/setup/UpdateAvailableEditor.h>
#include <proxies/hwui/panel-unit/boled/setup/VelocityEditor.h>
#include <proxies/hwui/panel-unit/boled/setup/VelocityView.h>
#include <proxies/hwui/panel-unit/boled/setup/USBStickAvailableView.h>
#include <proxies/hwui/panel-unit/boled/setup/ExportBackupView.h>
#include <proxies/hwui/panel-unit/boled/setup/ImportBackupView.h>
#include <proxies/hwui/panel-unit/boled/setup/ImportBackupEditor.h>
#include <proxies/hwui/panel-unit/boled/setup/SignalFlowIndicationView.h>
#include <proxies/hwui/panel-unit/boled/setup/SignalFlowIndicatorEditor.h>
#include <proxies/hwui/panel-unit/EditPanel.h>
#include <proxies/hwui/panel-unit/PanelUnit.h>
#include <proxies/lpc/LPCProxy.h>
#include <xml/FileOutStream.h>
#include <bitset>
#include <chrono>
#include <list>
#include <memory>

namespace NavTree
{
  struct InnerNode;

  struct Node
  {
      Node(InnerNode *parent, Glib::ustring name) :
          parent(parent),
          name(name)
      {
      }

      virtual Control *createSelectionControl()
      {
        return new SetupSelectionLabel(name);
      }

      virtual bool onEditModeEntered()
      {
        return false;
      }

      virtual Glib::ustring getName() const
      {
        return name;
      }

      InnerNode *parent;
      virtual Control *createView() = 0;

    protected:
      Glib::ustring name;
  };

  struct Leaf : Node
  {
      Leaf(InnerNode *parent, Glib::ustring name) :
          Node(parent, name)
      {
      }
  };

  struct EditableLeaf : Leaf
  {
      EditableLeaf(InnerNode *parent, Glib::ustring name) :
          Leaf(parent, name)
      {
      }

      virtual Control *createEditor() = 0;
  };

  struct InnerNode : Node
  {
      InnerNode(InnerNode *parent, Glib::ustring name) :
          Node(parent, name)
      {
      }

      virtual Control *createView() override
      {
        return new SetupLabel("...", Rect(0, 0, 0, 0));
      }

      list<unique_ptr<Node>> children;
  };

  struct Velocity : EditableLeaf
  {
      Velocity(InnerNode *parent) :
          EditableLeaf(parent, "Velocity Curve")
      {
      }

      virtual Control *createView() override
      {
        return new VelocityView();
      }

      virtual Control *createEditor() override
      {
        return new VelocityEditor();
      }
  };

  struct Aftertouch : EditableLeaf
  {
      Aftertouch(InnerNode *parent) :
          EditableLeaf(parent, "Aftertouch Curve")
      {
      }

      virtual Control *createView() override
      {
        return new AftertouchView();
      }

      virtual Control *createEditor() override
      {
        return new AftertouchEditor();
      }
  };

  struct BenderCurveSetting : EditableLeaf
  {
      BenderCurveSetting(InnerNode *parent) :
          EditableLeaf(parent, "Bender Curve")
      {
      }

      virtual Control *createView() override
      {
        return new BenderCurveView();
      }

      virtual Control *createEditor() override
      {
        return new BenderCurveEditor();
      }
  };

  struct PedalSetting : EditableLeaf
  {
      PedalSetting(InnerNode *parent, int id) :
          EditableLeaf(parent, "")
      {
        param = dynamic_cast<PedalParameter*>(Application::get().getPresetManager()->getEditBuffer()->findParameterByID(id));
        name = param->getLongName();
      }

      virtual Control *createView() override
      {
        return new PedalView(param->getAssociatedPedalTypeSetting());
      }

      virtual Control *createEditor() override
      {
        return new PedalEditor(param->getAssociatedPedalTypeSetting());
      }

      virtual Control *createSelectionControl()
      {
        return new PedalSelectionControl(param);
      }

      PedalParameter* param;

  };

  struct PresetGlitchSuppression : EditableLeaf
  {
      PresetGlitchSuppression(InnerNode *parent) :
          EditableLeaf(parent, "Preset Glitch Suppression ")
      {
      }

      virtual Control *createView() override
      {
        return new PresetGlitchSuppressionView();
      }

      virtual Control *createEditor() override
      {
        return new PresetGlitchSuppressionEditor();
      }
  };

  struct EditSmoothingTime : EditableLeaf
  {
      EditSmoothingTime(InnerNode *parent) :
          EditableLeaf(parent, "Edit Smoothing Time")
      {
      }

      virtual Control *createView() override
      {
        return new EditSmoothingTimeView();
      }

      virtual Control *createEditor() override
      {
        return new EditSmoothingTimeEditor();
      }
  };

  struct PedalSettings : InnerNode
  {
      PedalSettings(InnerNode *parent) :
          InnerNode(parent, "Pedals")
      {
        children.emplace_back(new PedalSetting(this, HardwareSourcesGroup::getPedal1ParameterID()));
        children.emplace_back(new PedalSetting(this, HardwareSourcesGroup::getPedal2ParameterID()));
        children.emplace_back(new PedalSetting(this, HardwareSourcesGroup::getPedal3ParameterID()));
        children.emplace_back(new PedalSetting(this, HardwareSourcesGroup::getPedal4ParameterID()));
      }
  };

  struct DeviceSettings : InnerNode
  {
      DeviceSettings(InnerNode *parent) :
          InnerNode(parent, "Device Settings")
      {
        children.emplace_back(new EditSmoothingTime(this));
        children.emplace_back(new Velocity(this));
        children.emplace_back(new Aftertouch(this));
        children.emplace_back(new BenderCurveSetting(this));
        children.emplace_back(new PedalSettings(this));
        children.emplace_back(new PresetGlitchSuppression(this));
      }
  };

  struct DeviceName : EditableLeaf
  {
      DeviceName(InnerNode *parent) :
          EditableLeaf(parent, "Device Name")
      {
      }

      virtual Control *createView() override
      {
        return new DeviceNameView();
      }

      virtual Control *createEditor() override
      {
        return nullptr;
      }

      virtual bool onEditModeEntered()
      {
        auto &boled = Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled();
        boled.setOverlay(new RenameDeviceLayout(boled.getLayout()));
        return true;
      }
  };

  struct SSID : Leaf
  {
      SSID(InnerNode *parent) :
          Leaf(parent, "SSID")
      {
      }

      virtual Control *createView() override
      {
        return new SSIDView();
      }
  };

  struct Passphrase : EditableLeaf
  {
      Passphrase(InnerNode *parent) :
          EditableLeaf(parent, "Passphrase")
      {
      }

      virtual Control *createView() override
      {
        return new PassphraseView();
      }

      virtual Control *createEditor() override
      {
        return new PassphraseEditor();
      }
  };

  struct UpdateAvailable : EditableLeaf
  {
      UpdateAvailable(InnerNode *parent) :
          EditableLeaf(parent, "Update Available")
      {
      }

      virtual Control *createView() override
      {
        return new UpdateAvailableView();
      }

      virtual Control *createEditor() override
      {
        if(UpdateAvailableView::updateExists())
          return new UpdateAvailableEditor();

        return nullptr;
      }
  };

  struct FreeInternalMemory : Leaf
  {
      FreeInternalMemory(InnerNode *parent) :
          Leaf(parent, "Free Internal Memory")
      {
      }

      virtual Control *createView() override
      {
        return new FreeInternalMemoryView();
      }
  };

  struct UISoftwareVersion : Leaf
  {
      UISoftwareVersion(InnerNode *parent) :
          Leaf(parent, "UI Software Version")
      {
      }

      virtual Control *createView() override
      {
        auto info = Application::get().getDeviceInformation()->getItem<::SoftwareVersion>().get();
        return new DeviceInfoItemView(info);
      }
  };

  struct RTSoftwareVersion : Leaf
  {
      RTSoftwareVersion(InnerNode *parent) :
          Leaf(parent, "RT Software Version")
      {
      }

      virtual Control *createView() override
      {
        Application::get().getLPCProxy()->requestLPCSoftwareVersion();
        auto info = Application::get().getDeviceInformation()->getItem<::RTSoftwareVersion>().get();
        return new DeviceInfoItemView(info);
      }
  };

  struct DateTime : EditableLeaf
  {
      DateTime(InnerNode *parent) :
          EditableLeaf(parent, "Date / Time")
      {
      }

      virtual Control *createView() override
      {
        auto info = Application::get().getDeviceInformation()->getItem<::DateTimeInfo>().get();
        return new DeviceInfoItemView(info, std::chrono::milliseconds(500));
      }

      virtual Control *createEditor() override
      {
        return new DateTimeEditor();
      }
  };

  struct OSVersion : Leaf
  {
      OSVersion(InnerNode *parent) :
          Leaf(parent, "OS Version")
      {
      }

      virtual Control *createView() override
      {
        auto info = Application::get().getDeviceInformation()->getItem<::OSVersion>().get();
        return new DeviceInfoItemView(info);
      }
  };

  struct SystemInfo : InnerNode
  {
      SystemInfo(InnerNode *parent) :
          InnerNode(parent, "System Info")
      {
        children.emplace_back(new DeviceName(this));
        children.emplace_back(new SSID(this));
        children.emplace_back(new Passphrase(this));
        children.emplace_back(new FreeInternalMemory(this));
        children.emplace_back(new UISoftwareVersion(this));
        children.emplace_back(new RTSoftwareVersion(this));
        children.emplace_back(new OSVersion(this));
        children.emplace_back(new DateTime(this));
        children.emplace_back(new UpdateAvailable(this));
      }
  };

  struct About : EditableLeaf
  {
      About(InnerNode *parent) :
          EditableLeaf(parent, "About")
      {
      }

      virtual Control *createView() override
      {
        return new SetupLabel("...", Rect(0, 0, 0, 0));
      }

      virtual Control *createEditor() override
      {
        return nullptr;
      }

      virtual bool onEditModeEntered()
      {
        auto &boled = Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled();
        boled.setOverlay(new AboutLayout());
        return true;
      }
  };

  struct EncoderAcceleration : EditableLeaf
  {
      EncoderAcceleration(InnerNode *parent) :
          EditableLeaf(parent, "Encoder Acceleration")
      {
      }

      virtual Control *createView() override
      {
        return new EncoderAccelerationView();
      }

      virtual Control *createEditor() override
      {
        return new EncoderAccelerationEditor();
      }
  };

  struct RibbonRelativeFactorSetting : EditableLeaf
  {
      RibbonRelativeFactorSetting(InnerNode *parent) :
          EditableLeaf(parent, "Ribbon Relative Factor")
      {
      }

      virtual Control *createView() override
      {
        return new RibbonRelativeFactorSettingView();
      }

      virtual Control *createEditor() override
      {
        return new RibbonRelativeFactorSettingEditor();
      }
  };

  struct SignalFlowIndicationSetting : EditableLeaf
  {
      SignalFlowIndicationSetting(InnerNode *parent) :
          EditableLeaf(parent, "Signal Flow Indication")
      {

      }

      virtual Control *createView() override
      {
        return new SignalFlowIndicationView();
      }

      virtual Control *createEditor() override
      {
        return new SignalFlowIndicatorEditor();
      }

  };

  struct HardwareUI : InnerNode
  {
      HardwareUI(InnerNode *parent) :
          InnerNode(parent, "Hardware UI")
      {
        children.emplace_back(new EncoderAcceleration(this));
        children.emplace_back(new RibbonRelativeFactorSetting(this));
        children.emplace_back(new SignalFlowIndicationSetting(this));
      }
  };

  struct USBStickAvailable : Leaf
  {
      USBStickAvailable(InnerNode *parent) :
          Leaf(parent, "USB Available")
      {
      }

      virtual Control *createView() override
      {
        return new USBStickAvailableView();
      }
  };

  struct BackupExport : EditableLeaf
  {
      BackupExport(InnerNode *parent) :
          EditableLeaf(parent, "Save all Banks...")
      {
      }

      virtual Control *createView() override
      {
        return new ExportBackupView();
      }

      virtual Control *createEditor() override
      {
        return new ExportBackupEditor();
      }
  };

  struct BackupImport : EditableLeaf
  {
      BackupImport(InnerNode *parent) :
          EditableLeaf(parent, "Restore all Banks...")
      {
      }

      virtual Control *createView() override
      {
        return new ImportBackupView();
      }

      virtual Control *createEditor() override
      {
        return new ImportBackupEditor();
      }
  };

  struct Backup : InnerNode
  {
      Backup(InnerNode* parent) :
          InnerNode(parent, "Backup")
      {
        children.emplace_back(new USBStickAvailable(this));
        children.emplace_back(new BackupExport(this));
        children.emplace_back(new BackupImport(this));
      }
  };

  struct Setup : InnerNode
  {
      Setup() :
          InnerNode(nullptr, "Setup")
      {
        children.emplace_back(new DeviceSettings(this));
        children.emplace_back(new HardwareUI(this));
        children.emplace_back(new SystemInfo(this));
        children.emplace_back(new About(this));
        children.emplace_back(new Backup(this));
        focus = children.begin();
      }

      Glib::ustring getName() const override
      {
        if(FileOutStream::getKioskMode())
          return "Setup (Kiosk)";

        return "Setup";
      }

      bool diveInto()
      {
        if(auto e = dynamic_cast<InnerNode*>((*focus).get()))
        {
          focus = e->children.begin();
          return true;
        }
        return false;
      }

      bool diveUp()
      {
        auto focusNode = (*focus).get();

        if(focusNode->parent && focusNode->parent->parent)
        {
          auto pa = focusNode->parent;
          auto grandpa = focusNode->parent->parent;
          for(auto it = grandpa->children.begin(); it != grandpa->children.end(); it++)
          {
            if(pa == it->get())
            {
              focus = it;
              return true;
            }
          }
        }
        return false;
      }

      void incFocus(int i)
      {
        auto p = (*focus)->parent;

        while(i > 0 && focus != p->children.end())
        {
          if(++focus == p->children.end())
            --focus;
          i--;
        }

        while(i < 0 && focus != p->children.begin())
        {
          focus--;
          i++;
        }
      }

      list<unique_ptr<Node>>::iterator focus;
  };
}

class Breadcrumb : public Control
{
  public:
    Breadcrumb(NavTree::Node *node) :
        Control(Rect(0, 0, 256, 12)),
        m_node(node)
    {
    }

    bool redraw(FrameBuffer &fb)
    {
      fb.setColor(FrameBuffer::C103);
      fb.fillRect(getPosition());
      drawNodeRecursivly(fb, m_node);
      return true;
    }

    int drawNodeRecursivly(FrameBuffer &fb, NavTree::Node *node)
    {
      if(node)
      {
        auto left = drawNodeRecursivly(fb, node->parent);
        auto isTip = node == m_node;
        auto title = node->getName();

        if(isTip)
        {
          fb.setColor(FrameBuffer::C255);
        }
        else
        {
          fb.setColor(FrameBuffer::C179);
          title = title + " > ";
        }

        auto font = Oleds::get().getFont("Emphase_9_Regular", 9);
        auto width = font->draw(title, left, getPosition().getBottom() - 1);
        return left + width;
      }
      return 5;
    }

  private:
    NavTree::Node *m_node;
};

SetupLayout::SetupLayout(FocusAndMode focusAndMode) :
    m_tree(new NavTree::Setup()),
    m_focusAndMode(focusAndMode)
{
  buildPage();
}

SetupLayout::~SetupLayout()
{
}

void SetupLayout::addBreadcrumb()
{
  auto focus = m_tree->focus->get();

  if(isInEditMode())
    addControl(new Breadcrumb(dynamic_cast<NavTree::Leaf*>(focus)));
  else
    addControl(new Breadcrumb(focus->parent));
}

void SetupLayout::buildPage()
{
  clear();
  m_editor = nullptr;

  addBreadcrumb();
  addSelectionEntries();

  if(!addEditor())
    addValueViews();

  finishLists();
}

void SetupLayout::addSelectionEntries()
{
  auto focus = m_tree->focus->get();
  auto focusParent = focus->parent;

  for(auto &c : focusParent->children)
  {
    Control *s = c->createSelectionControl();
    s->setHighlight((c.get() == focus));
    addSelectionEntry(s);
  }
}

bool SetupLayout::isInSelectionMode() const
{
  auto focus = m_tree->focus->get();
  auto focusEditable = dynamic_cast<const NavTree::Leaf*>(focus);
  return m_focusAndMode.mode == UIMode::Select || !focusEditable;
}

void SetupLayout::addValueViews()
{
  auto focus = m_tree->focus->get();
  auto focusParent = focus->parent;

  for(auto &c : focusParent->children)
  {
    bool isFocussed = c.get() == focus;
    addView(c->createView(), isFocussed);
  }
}

bool SetupLayout::isInEditMode() const
{
  auto focus = m_tree->focus->get();
  auto focusEditable = dynamic_cast<NavTree::Leaf*>(focus);
  return m_focusAndMode.mode == UIMode::Edit && focusEditable;
}

bool SetupLayout::addEditor()
{
  if(isInEditMode())
  {
    auto focus = m_tree->focus->get();

    if(auto leaf = dynamic_cast<NavTree::EditableLeaf*>(focus))
    {
      if(auto e = leaf->createEditor())
      {
        setEditor(e);
        return true;
      }
      else
      {
        m_focusAndMode.mode = UIMode::Select;
      }
    }
  }
  return false;
}

void SetupLayout::diveUp()
{
  if(m_focusAndMode.mode == UIMode::Edit)
    m_focusAndMode.mode = UIMode::Select;
  else
    m_tree->diveUp();

  buildPage();
}

bool SetupLayout::onButton(int i, bool down, ButtonModifiers modifiers)
{
  if(down)
  {
    if(i == BUTTON_PRESET)
    {
      Application::get().getHWUI()->undoableSetFocusAndMode(FocusAndMode(UIFocus::Presets, UIMode::Select));
      return true;
    }
    else if(i == BUTTON_STORE)
    {
      Application::get().getHWUI()->undoableSetFocusAndMode(FocusAndMode(UIFocus::Presets, UIMode::Store));
      return true;
    }

    if(m_focusAndMode.mode == UIMode::Select)
    {
      if(i == BUTTON_ENTER || i == BUTTON_C || i == BUTTON_D)
      {
        onEnterInSelectionMode(modifiers);
        return true;
      }
    }
    else if(m_editor)
    {
      if(!m_editor->onButton(i, down, modifiers))
      {
        if(i == BUTTON_ENTER)
        {
          onEnterInEditMode();
          return true;
        }
      }
    }

    if(i == BUTTON_A || i == BUTTON_B)
    {
      diveUp();
      return true;
    }
  }

  return super::onButton(i, down, modifiers);
}

void SetupLayout::onEnterInSelectionMode(ButtonModifiers modifiers)
{
  bool diveable = dynamic_cast<NavTree::EditableLeaf*>(m_tree->focus->get());
  diveable |= dynamic_cast<NavTree::InnerNode*>(m_tree->focus->get()) != nullptr;

  if(diveable)
  {
    if(!m_tree->diveInto())
    {
      m_focusAndMode.mode = UIMode::Edit;

      if(m_tree->focus->get()->onEditModeEntered())
      {
        m_focusAndMode.mode = UIMode::Select;
        return;
      }
    }
  }

  buildPage();
}

void SetupLayout::onEnterInEditMode()
{
  m_focusAndMode.mode = UIMode::Select;
  buildPage();
}

bool SetupLayout::onRotary(int inc, ButtonModifiers modifiers)
{
  if(m_focusAndMode.mode == UIMode::Select)
  {
    m_tree->incFocus(inc);
    buildPage();
    return true;
  }
  else if(m_editor)
  {
    return m_editor->onRotary(inc, modifiers);
  }
  return super::onRotary(inc, modifiers);
}

void SetupLayout::addSelectionEntry(Control *s)
{
  ensureSelectionEntries()->addEntry(s);
}

void SetupLayout::addView(Control *c, bool focussed)
{
  ensureViewEntries()->addEntry(c, focussed);
}

void SetupLayout::setEditor(Control *c)
{
  addControl(c);
  c->setPosition(Rect(129, 28, 126, 12));
  m_editor = dynamic_cast<SetupEditor*>(c);
  m_editor->setSetupLayout(this);
  g_assert(m_editor);
}

void SetupLayout::finishLists()
{
  if(auto r = findControlOfType<SetupSelectionEntries>())
    r->finish(m_focusAndMode.mode == UIMode::Select);

  if(auto r = findControlOfType<ViewEntries>())
    r->finish();
}

shared_ptr<SetupSelectionEntries> SetupLayout::ensureSelectionEntries()
{
  if(auto r = findControlOfType<SetupSelectionEntries>())
    return r;

  addControl(new SetupSelectionEntries(Rect(1, 16, 126, 48)));
  return ensureSelectionEntries();
}

shared_ptr<ViewEntries> SetupLayout::ensureViewEntries()
{
  if(auto r = findControlOfType<ViewEntries>())
    return r;

  addControl(new ViewEntries(Rect(129, 16, 126, 48)));
  return ensureViewEntries();
}

bool SetupLayout::redraw(FrameBuffer &fb)
{
  ControlOwner::redraw(fb);

  if(m_focusAndMode.mode == UIMode::Select)
  {
    fb.setColor(FrameBuffer::C179);
    fb.drawRect(Rect(0, 28, 256, 12));
  }
  return true;
}
