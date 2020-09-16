#include <Application.h>
#include <device-info/DateTimeInfo.h>
#include <device-info/DeviceInformation.h>
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
#include <proxies/hwui/panel-unit/boled/setup/WiFiSettingEditor.h>
#include <proxies/hwui/panel-unit/boled/setup/SettingView.h>
#include <proxies/hwui/panel-unit/boled/setup/EnumSettingEditor.h>
#include <proxies/hwui/panel-unit/boled/setup/NumericSettingEditor.h>
#include <proxies/hwui/panel-unit/boled/setup/WiFiSettingView.h>
#include <proxies/hwui/panel-unit/EditPanel.h>
#include <proxies/hwui/panel-unit/PanelUnit.h>
#include <proxies/hwui/TextEditUsageMode.h>
#include <proxies/playcontroller/PlaycontrollerProxy.h>
#include <xml/FileOutStream.h>
#include <bitset>
#include <chrono>
#include <list>
#include <memory>
#include <device-settings/TuneReference.h>
#include <device-settings/TransitionTime.h>
#include <tools/StringTools.h>
#include <parameter_declarations.h>
#include <device-settings/SyncVoiceGroupsAcrossUIS.h>
#include "UISoftwareVersionEditor.h"
#include "ScreenSaverTimeControls.h"
#include "SetupInfoTexts.h"
#include <proxies/hwui/descriptive-layouts/concrete/menu/menu-items/AnimatedGenericItem.h>

namespace NavTree
{
  struct InnerNode;

  struct Node
  {
    Node(InnerNode *parent, Glib::ustring name)
        : parent(parent)
        , name(name)
    {
    }

    virtual ~Node() = default;

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
    virtual Control *createInfoView() = 0;

   protected:
    Glib::ustring name;
  };

  struct PlaceHolderInfo : public ControlWithChildren
  {
    explicit PlaceHolderInfo(SetupInfoEntries entry)
        : ControlWithChildren(Rect(0, 0, 256, 96))
    {
      addControl(new Label({ SetupInfoHeaders.at(entry), 0 }, Rect(0, 0, 256, 12)));
      m_content = addControl(new MultiLineLabel(Rect(0, 12, 256, 84), SetupInfoContent.at(entry)));
    }

   private:
    MultiLineLabel *m_content;
  };

  struct Leaf : Node
  {
    Leaf(InnerNode *parent, Glib::ustring name)
        : Node(parent, name)
    {
    }
  };

  struct EditableLeaf : Leaf
  {
    EditableLeaf(InnerNode *parent, Glib::ustring name)
        : Leaf(parent, name)
    {
    }

    virtual Control *createEditor() = 0;
  };

  struct InnerNode : Node
  {
    InnerNode(InnerNode *parent, Glib::ustring name)
        : Node(parent, name)
    {
    }

    virtual Control *createView() override
    {
      return new SetupLabel("...", Rect(0, 0, 0, 0));
    }

    std::list<std::unique_ptr<Node>> children;
  };

  struct Velocity : EditableLeaf
  {
    Velocity(InnerNode *parent)
        : EditableLeaf(parent, "Velocity Curve")
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

    Control *createInfoView() override
    {
      return new PlaceHolderInfo(SetupInfoEntries::VelocityCurve);
    }
  };

  struct Aftertouch : EditableLeaf
  {
    Aftertouch(InnerNode *parent)
        : EditableLeaf(parent, "Aftertouch Curve")
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

    Control *createInfoView() override
    {
      return new PlaceHolderInfo(SetupInfoEntries::AftertouchCurve);
    }
  };

  struct BenderCurveSetting : EditableLeaf
  {
    BenderCurveSetting(InnerNode *parent)
        : EditableLeaf(parent, "Bender Curve")
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

    Control *createInfoView() override
    {
      return new PlaceHolderInfo(SetupInfoEntries::BenderCurve);
    }
  };

  struct PedalSetting : EditableLeaf
  {
    PedalSetting(InnerNode *parent, int id)
        : EditableLeaf(parent, "")
    {
      param = dynamic_cast<PedalParameter *>(
          Application::get().getPresetManager()->getEditBuffer()->findParameterByID({ id, VoiceGroup::Global }));
      name = param->getLongName();
    }

    Control *createView() override
    {
      return new PedalView(param->getAssociatedPedalTypeSetting());
    }

    Control *createEditor() override
    {
      return new PedalEditor(param->getAssociatedPedalTypeSetting());
    }

    Control *createSelectionControl() override
    {
      return new PedalSelectionControl(param);
    }

    Control *createInfoView() override
    {
      return new PlaceHolderInfo(SetupInfoEntries::PedalSetting);
    }

    PedalParameter *param;
  };

  struct WiFiSetting : EditableLeaf
  {
    WiFiSetting(InnerNode *parent)
        : EditableLeaf(parent, "Enable/Disable WiFi")
    {
    }

    Control *createView() override
    {
      return new WiFiSettingView();
    }

    Control *createEditor() override
    {
      return new WiFiSettingEditor();
    }

    Control *createInfoView() override
    {
      return new PlaceHolderInfo(SetupInfoEntries::WiFiSetting);
    }
  };

  struct OneShotEntry : EditableLeaf
  {
    using CB = std::function<void()>;
    struct Item : public AnimatedGenericItem
    {
      Item(const Rect &rect, const CB &cb, SetupInfoEntries e)
          : AnimatedGenericItem("", rect, cb)
          , m_entry { e }
      {
      }

      bool drawHighlightBorder(FrameBuffer &) override
      {
        return false;
      }

      Control *createInfo() override
      {
        return new PlaceHolderInfo(m_entry);
      }

     private:
      SetupInfoEntries m_entry;
    };

    Item *theItem = nullptr;
    CB cb;

    OneShotEntry(InnerNode *p, const std::string &name, const CB &cb, SetupInfoEntries e)
        : EditableLeaf(p, name)
        , cb(cb)
        , m_entry { e }
    {
    }

    Control *createView() override
    {
      theItem = new Item(Rect(0, 0, 0, 0), cb, m_entry);
      return theItem;
    }

    Control *createEditor() override
    {
      return nullptr;
    }

    bool onEditModeEntered() override
    {
      if(theItem)
        theItem->doAction();
      return true;
    }

    Control *createInfoView() override
    {
      return new PlaceHolderInfo(m_entry);
    }

   private:
    SetupInfoEntries m_entry;
  };

  struct StoreInitSound : OneShotEntry
  {
    StoreInitSound(InnerNode *p)
        : OneShotEntry(p, "Store Init Sound",
                       [] {
                         auto pm = Application::get().getPresetManager();
                         auto scope = pm->getUndoScope().startTransaction("Store Init Sound");
                         pm->storeInitSound(scope->getTransaction());
                       },
                       SetupInfoEntries::StoreInitSound)
    {
    }
  };

  struct ResetInitSound : OneShotEntry
  {
    ResetInitSound(InnerNode *p)
        : OneShotEntry(p, "Reset Init Sound",
                       [] {
                         auto pm = Application::get().getPresetManager();
                         auto scope = pm->getUndoScope().startTransaction("Reset Init Sound");
                         pm->resetInitSound(scope->getTransaction());
                       },
                       SetupInfoEntries::ResetInitSound)
    {
    }
  };

  template <typename tSetting> struct SettingItem : EditableLeaf
  {
   private:
    tSetting *getSetting()
    {
      return Application::get().getSettings()->getSetting<tSetting>().get();
    }

   public:
    SettingItem(InnerNode *parent, const char *name)
        : EditableLeaf(parent, name)
    {
    }

    Control *createView() override
    {
      return new SettingView<tSetting>();
    }

    Control *createEditor() override
    {
      if constexpr(std::is_base_of_v<BooleanSetting, tSetting>)
      {
        return new BooleanSettingEditor<tSetting>();
      }
      else
      {
        return new NumericSettingEditor<tSetting>();
      }
    }

    Control *createInfoView() override
    {
      return new PlaceHolderInfo(SettingToSetupInfoEntry<tSetting>());
    }
  };

  struct PresetGlitchSuppression : EditableLeaf
  {
    PresetGlitchSuppression(InnerNode *parent)
        : EditableLeaf(parent, "Preset Glitch Suppression ")
    {
    }

    Control *createView() override
    {
      return new PresetGlitchSuppressionView();
    }

    Control *createEditor() override
    {
      return new PresetGlitchSuppressionEditor();
    }

    Control *createInfoView() override
    {
      return new PlaceHolderInfo(SetupInfoEntries::PresetGlitchSuppression);
    }
  };

  struct EditSmoothingTime : EditableLeaf
  {
    EditSmoothingTime(InnerNode *parent)
        : EditableLeaf(parent, "Edit Smoothing Time")
    {
    }

    Control *createView() override
    {
      return new EditSmoothingTimeView();
    }

    Control *createEditor() override
    {
      return new EditSmoothingTimeEditor();
    }

    Control *createInfoView() override
    {
      return new PlaceHolderInfo(SetupInfoEntries::EditSmoothingTime);
    }
  };

  struct PedalSettings : InnerNode
  {
    PedalSettings(InnerNode *parent)
        : InnerNode(parent, "Pedals")
    {
      children.emplace_back(new PedalSetting(this, HardwareSourcesGroup::getPedal1ParameterID().getNumber()));
      children.emplace_back(new PedalSetting(this, HardwareSourcesGroup::getPedal2ParameterID().getNumber()));
      children.emplace_back(new PedalSetting(this, HardwareSourcesGroup::getPedal3ParameterID().getNumber()));
      children.emplace_back(new PedalSetting(this, HardwareSourcesGroup::getPedal4ParameterID().getNumber()));
    }

    Control *createInfoView() override
    {
      return new PlaceHolderInfo(SetupInfoEntries::Pedals);
    }
  };

  struct DeviceSettings : InnerNode
  {
    DeviceSettings(InnerNode *parent)
        : InnerNode(parent, "Device Settings")
    {
      children.emplace_back(new EditSmoothingTime(this));
      children.emplace_back(new SettingItem<TuneReference>(this, "Tune Reference"));
      children.emplace_back(new SettingItem<TransitionTime>(this, "Transition Time"));
      children.emplace_back(new Velocity(this));
      children.emplace_back(new Aftertouch(this));
      children.emplace_back(new BenderCurveSetting(this));
      children.emplace_back(new PedalSettings(this));
      children.emplace_back(new PresetGlitchSuppression(this));
      children.emplace_back(new SettingItem<SyncVoiceGroupsAcrossUIS>(this, "Sync Parts across UIs"));
      children.emplace_back(new WiFiSetting(this));
      children.emplace_back(new StoreInitSound(this));
      children.emplace_back(new ResetInitSound(this));
    }

    Control *createInfoView() override
    {
      return new PlaceHolderInfo(SetupInfoEntries::DeviceSettings);
    }
  };

  struct DeviceName : EditableLeaf
  {
    DeviceName(InnerNode *parent)
        : EditableLeaf(parent, "Device Name")
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

    Control *createInfoView() override
    {
      return new PlaceHolderInfo(SetupInfoEntries::DeviceName);
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
    SSID(InnerNode *parent)
        : Leaf(parent, "SSID")
    {
    }

    virtual Control *createView() override
    {
      return new SSIDView();
    }

    Control *createInfoView() override
    {
      return new PlaceHolderInfo(SetupInfoEntries::SSID);
    }
  };

  struct Passphrase : EditableLeaf
  {
    Passphrase(InnerNode *parent)
        : EditableLeaf(parent, "Passphrase")
    {
    }

    virtual Control *createView() override
    {
      return new PassphraseView();
    }

    Control *createInfoView() override
    {
      return new PlaceHolderInfo(SetupInfoEntries::Passphrase);
    }

    virtual Control *createEditor() override
    {
      return new PassphraseEditor();
    }
  };

  struct UpdateAvailable : EditableLeaf
  {
    UpdateAvailable(InnerNode *parent)
        : EditableLeaf(parent, "Update Available")
    {
    }

    virtual Control *createView() override
    {
      return new UpdateAvailableView();
    }

    Control *createInfoView() override
    {
      return new PlaceHolderInfo(SetupInfoEntries::UpdateAvailable);
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
    FreeInternalMemory(InnerNode *parent)
        : Leaf(parent, "Free Internal Memory")
    {
    }

    virtual Control *createView() override
    {
      return new FreeInternalMemoryView();
    }

    Control *createInfoView() override
    {
      return new PlaceHolderInfo(SetupInfoEntries::FreeMemory);
    }
  };

  struct UISoftwareVersion : EditableLeaf
  {
    UISoftwareVersion(InnerNode *parent)
        : EditableLeaf(parent, "Software Version")
    {
    }

    Control *createInfoView() override
    {
      return new PlaceHolderInfo(SetupInfoEntries::SoftwareVersion);
    }

    virtual Control *createView() override
    {
      auto info = Application::get().getDeviceInformation()->getItem<::SoftwareVersion>().get();
      return new DeviceInfoItemView(info);
    }

    virtual Control *createEditor() override
    {
      return new UISoftwareVersionEditor();
    }
  };

  struct DateTime : EditableLeaf
  {
    DateTime(InnerNode *parent)
        : EditableLeaf(parent, "Date / Time")
    {
    }

    virtual Control *createView() override
    {
      auto info = Application::get().getDeviceInformation()->getItem<::DateTimeInfo>().get();
      return new DeviceInfoItemView(info, std::chrono::milliseconds(500));
    }

    Control *createInfoView() override
    {
      return new PlaceHolderInfo(SetupInfoEntries::DateTime);
    }

    virtual Control *createEditor() override
    {
      return new DateTimeEditor();
    }
  };

  struct WebUIAdress : Leaf
  {
    struct AddressLabel : public SetupLabel
    {
      AddressLabel()
          : SetupLabel("192.168.8.2", Rect(0, 0, 0, 0))
      {
      }
    };

    Control *createInfoView() override
    {
      return new PlaceHolderInfo(SetupInfoEntries::WebsiteAddress);
    }

    WebUIAdress(InnerNode *parent)
        : Leaf(parent, "Website Address:")
    {
    }

    virtual Control *createView() override
    {
      return new AddressLabel();
    }
  };

  struct SystemInfo : InnerNode
  {
    SystemInfo(InnerNode *parent)
        : InnerNode(parent, "System Info")
    {
      children.emplace_back(new DeviceName(this));
      children.emplace_back(new SSID(this));
      children.emplace_back(new Passphrase(this));
      children.emplace_back(new WebUIAdress(this));
      children.emplace_back(new FreeInternalMemory(this));
      children.emplace_back(new UISoftwareVersion(this));
      children.emplace_back(new DateTime(this));
      children.emplace_back(new UpdateAvailable(this));
    }

    Control *createInfoView() override
    {
      return new PlaceHolderInfo(SetupInfoEntries::SystemInfo);
    }
  };

  struct About : EditableLeaf
  {
    About(InnerNode *parent)
        : EditableLeaf(parent, "About")
    {
    }

    Control *createInfoView() override
    {
      return new PlaceHolderInfo(SetupInfoEntries::About);
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
    EncoderAcceleration(InnerNode *parent)
        : EditableLeaf(parent, "Encoder Acceleration")
    {
    }

    Control *createInfoView() override
    {
      return new PlaceHolderInfo(SetupInfoEntries::EncoderAcceleration);
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
    RibbonRelativeFactorSetting(InnerNode *parent)
        : EditableLeaf(parent, "Ribbon Relative Factor")
    {
    }

    Control *createInfoView() override
    {
      return new PlaceHolderInfo(SetupInfoEntries::RibbonRelativeFactor);
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
    SignalFlowIndicationSetting(InnerNode *parent)
        : EditableLeaf(parent, "Signal Flow Indication")
    {
    }

    virtual Control *createView() override
    {
      return new SignalFlowIndicationView();
    }

    Control *createInfoView() override
    {
      return new PlaceHolderInfo(SetupInfoEntries::SignalFlowIndicator);
    }

    virtual Control *createEditor() override
    {
      return new SignalFlowIndicatorEditor();
    }
  };

  struct ScreenSaverTime : EditableLeaf
  {
    ScreenSaverTime(InnerNode *parent)
        : EditableLeaf(parent, "Screensaver Timeout")
    {
    }

    Control *createInfoView() override
    {
      return new PlaceHolderInfo(SetupInfoEntries::ScreenSaverTimeout);
    }

    Control *createView() override
    {
      return new ScreenSaverTimeView();
    }

    Control *createEditor() override
    {
      return new ScreenSaverTimeEditor();
    }
  };

  struct HardwareUI : InnerNode
  {
    HardwareUI(InnerNode *parent)
        : InnerNode(parent, "Hardware UI")
    {
      children.emplace_back(new EncoderAcceleration(this));
      children.emplace_back(new RibbonRelativeFactorSetting(this));
      children.emplace_back(new SignalFlowIndicationSetting(this));
      children.emplace_back(new ScreenSaverTime(this));
    }

    Control *createInfoView() override
    {
      return new PlaceHolderInfo(SetupInfoEntries::HardwareUI);
    }
  };

  struct USBStickAvailable : Leaf
  {
    USBStickAvailable(InnerNode *parent)
        : Leaf(parent, "USB Available")
    {
    }

    virtual Control *createView() override
    {
      return new USBStickAvailableView();
    }

    Control *createInfoView() override
    {
      return new PlaceHolderInfo(SetupInfoEntries::USBAvailable);
    }
  };

  struct BackupExport : EditableLeaf
  {
    BackupExport(InnerNode *parent)
        : EditableLeaf(parent, "Save all Banks...")
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

    Control *createInfoView() override
    {
      return new PlaceHolderInfo(SetupInfoEntries::SaveAllBanks);
    }
  };

  struct BackupImport : EditableLeaf
  {
    BackupImport(InnerNode *parent)
        : EditableLeaf(parent, "Restore all Banks...")
    {
    }

    Control *createInfoView() override
    {
      return new PlaceHolderInfo(SetupInfoEntries::RestoreAllBanks);
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
    Backup(InnerNode *parent)
        : InnerNode(parent, "Backup")
    {
      children.emplace_back(new USBStickAvailable(this));
      children.emplace_back(new BackupExport(this));
      children.emplace_back(new BackupImport(this));
    }

    Control *createInfoView() override
    {
      return new PlaceHolderInfo(SetupInfoEntries::Backup);
    }
  };

  struct Setup : InnerNode
  {
    Setup()
        : InnerNode(nullptr, "Setup")
    {
      children.emplace_back(new DeviceSettings(this));
      children.emplace_back(new HardwareUI(this));
      children.emplace_back(new SystemInfo(this));
      children.emplace_back(new About(this));
      children.emplace_back(new Backup(this));
      focus = children.begin();
    }

    Control *createInfoView() override
    {
      return new PlaceHolderInfo(SetupInfoEntries::Setup);
    }

    Glib::ustring getName() const override
    {
      if(FileOutStream::getKioskMode())
        return "Setup (Kiosk)";

      return "Setup";
    }

    bool diveInto()
    {
      if(auto e = dynamic_cast<InnerNode *>((*focus).get()))
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

    std::list<std::unique_ptr<Node>>::iterator focus;
  };
}

class Breadcrumb : public Control
{
 public:
  Breadcrumb(NavTree::Node *node)
      : Control(Rect(0, 0, 256, 12))
      , m_node(node)
  {
  }

  bool redraw(FrameBuffer &fb)
  {
    fb.setColor(FrameBufferColors::C103);
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
        fb.setColor(FrameBufferColors::C255);
      }
      else
      {
        fb.setColor(FrameBufferColors::C179);
        title = title + " > ";
      }

      auto font = Oleds::get().getFont("Emphase-9-Regular", 9);
      auto width = font->draw(title, left, getPosition().getBottom() - 1);
      return left + width;
    }
    return 5;
  }

 private:
  NavTree::Node *m_node;
};

SetupLayout::SetupLayout(FocusAndMode focusAndMode)
    : super(Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled())
    , m_tree(new NavTree::Setup())
    , m_focusAndMode(focusAndMode)
{
  buildPage();
}

SetupLayout::~SetupLayout() = default;

void SetupLayout::addBreadcrumb()
{
  auto focus = m_tree->focus->get();

  if(isInEditMode())
    addControl(new Breadcrumb(dynamic_cast<NavTree::Leaf *>(focus)));
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
  auto focusEditable = dynamic_cast<NavTree::Leaf *>(focus);
  return m_focusAndMode.mode == UIMode::Edit && focusEditable;
}

bool SetupLayout::addEditor()
{
  if(isInEditMode())
  {
    auto focus = m_tree->focus->get();

    if(auto leaf = dynamic_cast<NavTree::EditableLeaf *>(focus))
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

bool SetupLayout::onButton(Buttons i, bool down, ButtonModifiers modifiers)
{
  if(i == Buttons::BUTTON_INFO && down)
  {
    toggleInfo();
    return true;
  }

  if(down)
  {
    if(i == Buttons::BUTTON_PRESET)
    {
      Application::get().getHWUI()->undoableSetFocusAndMode(FocusAndMode(UIFocus::Presets, UIMode::Select));
      return true;
    }
    else if(i == Buttons::BUTTON_STORE)
    {
      Application::get().getHWUI()->undoableSetFocusAndMode(FocusAndMode(UIFocus::Presets, UIMode::Store));
      return true;
    }

    if(m_focusAndMode.mode == UIMode::Select)
    {
      if(i == Buttons::BUTTON_ENTER || i == Buttons::BUTTON_C || i == Buttons::BUTTON_D)
      {
        onEnterInSelectionMode(modifiers);
        return true;
      }
    }
    else if(m_editor)
    {
      if(!m_editor->onButton(i, down, modifiers))
      {
        if(i == Buttons::BUTTON_ENTER)
        {
          onEnterInEditMode();
          return true;
        }
      }
    }

    if(i == Buttons::BUTTON_A || i == Buttons::BUTTON_B)
    {
      diveUp();
      return true;
    }
  }

  return super::onButton(i, down, modifiers);
}

void SetupLayout::onEnterInSelectionMode(ButtonModifiers modifiers)
{
  bool diveable = dynamic_cast<NavTree::EditableLeaf *>(m_tree->focus->get());
  diveable |= dynamic_cast<NavTree::InnerNode *>(m_tree->focus->get()) != nullptr;

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
  m_editor = dynamic_cast<SetupEditor *>(c);
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

std::shared_ptr<SetupSelectionEntries> SetupLayout::ensureSelectionEntries()
{
  if(auto r = findControlOfType<SetupSelectionEntries>())
    return r;

  addControl(new SetupSelectionEntries(Rect(1, 16, 126, 48)));
  return ensureSelectionEntries();
}

std::shared_ptr<ViewEntries> SetupLayout::ensureViewEntries()
{
  if(auto r = findControlOfType<ViewEntries>())
    return r;

  addControl(new ViewEntries(Rect(129, 16, 126, 48)));
  return ensureViewEntries();
}

bool SetupLayout::redraw(FrameBuffer &fb)
{
  ControlOwner::redraw(fb);

  if(m_focusAndMode.mode == UIMode::Select && m_infoLayout == nullptr)
  {
    fb.setColor(FrameBufferColors::C179);
    fb.drawRect(Rect(0, 28, 256, 12));
  }
  return true;
}

void SetupLayout::toggleInfo()
{
  if(m_infoLayout)
  {
    remove(m_infoLayout);
    m_infoLayout = nullptr;
  }
  else
  {
    if(*m_tree->focus)
    {
      m_infoLayout = addControl(m_tree->focus->get()->createInfoView());
    }
  }
}
