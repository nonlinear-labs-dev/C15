#include <Application.h>
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
#include <proxies/hwui/panel-unit/boled/setup/SettingEditors.h>
#include <proxies/hwui/panel-unit/boled/setup/NumericSettingEditor.h>
#include <proxies/hwui/panel-unit/boled/setup/WiFiSettingView.h>
#include <proxies/hwui/panel-unit/EditPanel.h>
#include <proxies/hwui/panel-unit/PanelUnit.h>
#include <proxies/hwui/TextEditUsageMode.h>
#include <proxies/hwui/controls/LeftAlignedLabel.h>
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
#include "MenuEditorEntry.h"
#include "HardwareEnableSettingsView.h"
#include "HardwareEnableSettingsEditor.h"
#include "HardwareEnableSelectionControl.h"

#include <proxies/hwui/descriptive-layouts/concrete/menu/menu-items/AnimatedGenericItem.h>
#include <device-settings/midi/MidiChannelSettings.h>
#include <device-settings/midi/receive/MidiReceiveVelocityCurveSetting.h>
#include <device-settings/midi/receive/MidiReceiveAftertouchCurveSetting.h>
#include <device-settings/midi/local/LocalNotesSetting.h>
#include <device-settings/midi/mappings/PedalCCMapping.h>
#include <device-settings/midi/mappings/RibbonCCMapping.h>
#include <device-settings/midi/mappings/BenderCCMapping.h>
#include <device-settings/midi/mappings/AftertouchCCMapping.h>
#include <device-settings/midi/mappings/EnableHighVelocityCC.h>
#include <device-settings/midi/mappings/Enable14BitSupport.h>
#include <device-settings/flac/AutoStartRecorderSetting.h>

#include <presets/Bank.h>
#include <use-cases/SettingsUseCases.h>
#include <device-settings/ScreenSaverTimeoutSetting.h>
#include <device-settings/UsedRAM.h>
#include <device-settings/TotalRAM.h>

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

   protected:
    Glib::ustring name;
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

  template <typename tSetting> struct EnumSettingItem : EditableLeaf
  {
    EnumSettingItem(InnerNode *parent, const std::string &text)
        : EditableLeaf(parent, text)
    {
    }

    Control *createView() override
    {
      return new SettingView<tSetting>();
    }

    Control *createEditor() override
    {
      return new EnumSettingEditor<tSetting>();
    }
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
  };

  struct OneShotEntry : EditableLeaf
  {
    using CB = std::function<void()>;
    struct Item : public AnimatedGenericItem
    {
      Item(const Rect &rect, const CB &cb)
          : AnimatedGenericItem("", rect, cb)
      {
      }

      bool drawHighlightBorder(FrameBuffer &) override
      {
        return false;
      }
    };

    Item *theItem = nullptr;
    CB cb;

    OneShotEntry(InnerNode *p, const std::string &name, const CB &cb)
        : EditableLeaf(p, name)
        , cb(cb)
    {
    }

    Control *createView() override
    {
      theItem = new Item(Rect(0, 0, 0, 0), cb);
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
  };

  struct StoreInitSound : OneShotEntry
  {
    StoreInitSound(InnerNode *p)
        : OneShotEntry(p, "Store Init Sound", [] {
          auto pm = Application::get().getPresetManager();
          SoundUseCases useCases(pm->getEditBuffer(), pm);
          useCases.storeInitSound();
        })
    {
    }
  };

  struct ResetInitSound : OneShotEntry
  {
    ResetInitSound(InnerNode *p)
        : OneShotEntry(p, "Reset Init Sound", [] {
          auto pm = Application::get().getPresetManager();
          SoundUseCases useCases(pm->getEditBuffer(), pm);
          useCases.resetInitSound();
        })
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
      children.emplace_back(new EnumSettingItem<SyncVoiceGroupsAcrossUIS>(this, "Sync Parts across UIs"));
      children.emplace_back(new WiFiSetting(this));
      children.emplace_back(new StoreInitSound(this));
      children.emplace_back(new ResetInitSound(this));
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
  };

  struct UISoftwareVersion : EditableLeaf
  {
    UISoftwareVersion(InnerNode *parent)
        : EditableLeaf(parent, "Software Version")
    {
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
    WebUIAdress(InnerNode *parent)
        : Leaf(parent, "Website Address:")
    {
    }
    virtual Control *createView() override
    {
      return new AddressLabel();
    }
  };

  struct RamUsage : Leaf
  {
    struct RamUsageLabel : public SetupLabel
    {
      RamUsageLabel()
          : SetupLabel("", Rect(0, 0, 0, 0))
      {

        m_connection = Application::get().getSettings()->getSetting<UsedRAM>()->onChange(
            sigc::mem_fun(this, &RamUsageLabel::onSettingChanged));
      }

      void onSettingChanged(const Setting *s)
      {
        if(auto used = dynamic_cast<const UsedRAM *>(s))
        {
          auto settings = Application::get().getSettings();
          auto total = settings->getSetting<TotalRAM>();
          StringAndSuffix str { used->getDisplayString() + " / " + total->getDisplayString() + " MB", 0 };
          setText(str);
        }
      }

     private:
      sigc::connection m_connection;
    };

    RamUsage(InnerNode *parent)
        : Leaf(parent, "RAM usage:")
    {
    }

    Control *createView() override
    {
      return new RamUsageLabel();
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
      children.emplace_back(new RamUsage(this));
      children.emplace_back(new UISoftwareVersion(this));
      children.emplace_back(new DateTime(this));
      children.emplace_back(new UpdateAvailable(this));
    }
  };

  struct About : EditableLeaf
  {
    About(InnerNode *parent)
        : EditableLeaf(parent, "About")
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
    EncoderAcceleration(InnerNode *parent)
        : EditableLeaf(parent, "Encoder Acceleration")
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
    RibbonRelativeFactorSetting(InnerNode *parent)
        : EditableLeaf(parent, "Ribbon Relative Factor")
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
    SignalFlowIndicationSetting(InnerNode *parent)
        : EditableLeaf(parent, "Signal Flow Indication")
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

  struct ScreenSaverTime : EditableLeaf
  {
    ScreenSaverTime(InnerNode *parent)
        : EditableLeaf(parent, "Screensaver Timeout")
    {
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
  };

  struct BackupImport : EditableLeaf
  {
    BackupImport(InnerNode *parent)
        : EditableLeaf(parent, "Restore all Banks...")
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
    Backup(InnerNode *parent)
        : InnerNode(parent, "Backup")
    {
      children.emplace_back(new USBStickAvailable(this));
      children.emplace_back(new BackupExport(this));
      children.emplace_back(new BackupImport(this));
    }
  };

  struct SettingView : SetupLabel
  {
    SettingView()
        : SetupLabel("...", Rect(0, 0, 0, 0))
    {
      setText("...");
    }
  };

  struct MidiReceiveSettings : InnerNode
  {
    MidiReceiveSettings(InnerNode *parent)
        : InnerNode(parent, "Receive")
    {
      children.emplace_back(new EnumSettingItem<MidiReceiveChannelSetting>(this, "Channel"));
      children.emplace_back(new EnumSettingItem<MidiReceiveChannelSplitSetting>(this, "Split Channel (Part II)"));
    }
  };

  struct MidiSendSettings : InnerNode
  {
    MidiSendSettings(InnerNode *parent)
        : InnerNode(parent, "Send")
    {
      children.emplace_back(new EnumSettingItem<MidiSendChannelSetting>(this, "Channel"));
      children.emplace_back(new EnumSettingItem<MidiSendChannelSplitSetting>(this, "Split Channel (Part II)"));
    }
  };

  struct MidiLocalSettings : InnerNode
  {
    MidiLocalSettings(InnerNode *parent)
        : InnerNode(parent, "Local")
    {
      children.emplace_back(new EnumSettingItem<LocalNotesSetting>(this, "Enable Notes"));
    }
  };

  struct MidiMappingSettings : InnerNode
  {
    MidiMappingSettings(InnerNode *parent)
        : InnerNode { parent, "Mappings" }
    {
      children.emplace_back(new EnumSettingItem<PedalCCMapping<1>>(this, "Pedal 1"));
      children.emplace_back(new EnumSettingItem<PedalCCMapping<2>>(this, "Pedal 2"));
      children.emplace_back(new EnumSettingItem<PedalCCMapping<3>>(this, "Pedal 3"));
      children.emplace_back(new EnumSettingItem<PedalCCMapping<4>>(this, "Pedal 4"));
      children.emplace_back(new EnumSettingItem<RibbonCCMapping<1>>(this, "Ribbon 1"));
      children.emplace_back(new EnumSettingItem<RibbonCCMapping<2>>(this, "Ribbon 2"));
      children.emplace_back(new EnumSettingItem<BenderCCMapping>(this, "Bender"));
      children.emplace_back(new EnumSettingItem<AftertouchCCMapping>(this, "Aftertouch"));
      children.emplace_back(new EnumSettingItem<EnableHighVelocityCC>(this, "High-Res. Velocity (CC 88)"));
      children.emplace_back(new EnumSettingItem<Enable14BitSupport>(this, "High-Res. CCs (use LSB)"));
    }
  };

  struct MidiProgramChangeBank : EditableLeaf
  {
   private:
    struct View : SetupLabel
    {
      View()
          : SetupLabel({ 0, 0, 0, 0 })
      {
      }

      StringAndSuffix getText() const override
      {
        auto pm = Application::get().getPresetManager();
        auto selected = pm->getMidiSelectedBank();
        auto selectedBank = pm->findBank(selected);

        if(selectedBank)
        {
          auto pos = pm->getBankPosition(selected);
          return { std::to_string(pos + 1) + "-" + selectedBank->getName(true), 0 };
        }
        else
        {
          return { "none", 0 };
        }
      }
    };

    struct Editor : MenuEditor
    {
      Editor()
          : MenuEditor()
      {
        m_midiSelectionChanged
            = getPresetManager()->onMidiBankSelectionHappened([this](auto uuid) { updateOnSettingChanged(); });
      }

      ~Editor() override
      {
        m_midiSelectionChanged.disconnect();
      }

     protected:
      sigc::connection m_midiSelectionChanged;
      static PresetManager *getPresetManager()
      {
        return Application::get().getPresetManager();
      }

      static Bank *getMidiBank()
      {
        return getPresetManager()->findMidiSelectedBank();
      }

      void incSetting(int inc) override
      {
        auto pm = getPresetManager();
        PresetManagerUseCases useCase(pm);
        const auto numBanks = pm->getNumBanks();

        if(auto current = getMidiBank())
        {
          auto currentIndex = getSelectedIndex();
          auto newIndex = currentIndex + inc;
          auto size = getDisplayStrings().size();

          newIndex %= static_cast<int>(size);

          if(newIndex != 0)
            useCase.selectMidiBank(pm->getBankAt(newIndex - 1));
          else
            useCase.selectMidiBank(nullptr);
        }
        else if(numBanks > 0)
        {
          if(inc > 0)
            useCase.selectMidiBank(pm->getBankAt(0));
          else if(inc < 0)
            useCase.selectMidiBank(pm->getBankAt(pm->getNumBanks() - 1));
        }
      }

      const std::vector<Glib::ustring> &getDisplayStrings() const override
      {
        static std::vector<Glib::ustring> sBankStrings;
        auto pm = Application::get().getPresetManager();
        int displayIndex = 1;
        sBankStrings.clear();
        sBankStrings.emplace_back("None");
        for(auto b : pm->getBanks())
        {
          sBankStrings.emplace_back(std::to_string(displayIndex) + "-" + b->getName(true));
          displayIndex++;
        }
        return sBankStrings;
      }

      int getSelectedIndex() const override
      {
        if(auto sel = getMidiBank())
          return static_cast<int>(getPresetManager()->getBankPosition(sel->getUuid()) + 1);
        else
          return 0;
      }
    };

   public:
    explicit MidiProgramChangeBank(InnerNode *n)
        : EditableLeaf(n, "Program Change Bank \uE0C1")
    {
    }

    Control *createView() override
    {
      return new View();
    }

    Control *createEditor() override
    {
      return new Editor();
    }
  };

  struct ResetMidiSettingsToHighRes : public OneShotEntry
  {

    explicit ResetMidiSettingsToHighRes(InnerNode *parent)
        : OneShotEntry(parent, getName(), []() {
          SettingsUseCases useCases(Application::get().getSettings());
          useCases.setMappingsToHighRes();
        })
    {
    }

    constexpr const char *getName()
    {
      return "Set to High-Res. Defaults";
    }
  };

  struct ResetMidiSettingsToClassic : public OneShotEntry
  {

    explicit ResetMidiSettingsToClassic(InnerNode *parent)
        : OneShotEntry(parent, getName(), []() {
          SettingsUseCases useCases(Application::get().getSettings());
          useCases.setMappingsToClassicMidi();
        })
    {
    }

    constexpr const char *getName()
    {
      return "Set to Classic MIDI Defaults";
    }
  };

  struct MidiPanicButton : OneShotEntry
  {
    MidiPanicButton(InnerNode *p)
        : OneShotEntry(p, "Panic Button", []() {
          SettingsUseCases useCase(Application::get().getSettings());
          useCase.panicAudioEngine();
        })
    {
    }
  };

  template <HW_SOURCE_IDS hw> struct HardwareEnableSetting : public EditableLeaf
  {
   public:
    HardwareEnableSetting(InnerNode *p, const Glib::ustring &text)
        : EditableLeaf(p, text)
    {
    }

    Control *createView() override
    {
      return new LeftAlignedLabel({ "..." }, Rect { 0, 0, 128, 16 });
    }

    Control *createEditor() override
    {
      return new HardwareEnableSettingsEditor<hw>();
    }
  };

  struct HardwareEnableSettings : InnerNode
  {
    explicit HardwareEnableSettings(InnerNode *p)
        : InnerNode(p, "Hardware Source Settings")
    {
      children.emplace_back(new HardwareEnableSetting<HW_SOURCE_ID_PEDAL_1>(this, "Pedal 1"));
      children.emplace_back(new HardwareEnableSetting<HW_SOURCE_ID_PEDAL_2>(this, "Pedal 2"));
      children.emplace_back(new HardwareEnableSetting<HW_SOURCE_ID_PEDAL_3>(this, "Pedal 3"));
      children.emplace_back(new HardwareEnableSetting<HW_SOURCE_ID_PEDAL_4>(this, "Pedal 4"));
      children.emplace_back(new HardwareEnableSetting<HW_SOURCE_ID_RIBBON_1>(this, "Ribbon 1"));
      children.emplace_back(new HardwareEnableSetting<HW_SOURCE_ID_RIBBON_2>(this, "Ribbon 2"));
      children.emplace_back(new HardwareEnableSetting<HW_SOURCE_ID_AFTERTOUCH>(this, "Aftertouch"));
      children.emplace_back(new HardwareEnableSetting<HW_SOURCE_ID_PITCHBEND>(this, "Bender"));
    }

    Control *createView() override
    {
      return new HardwareEnableSettingsView();
    }
  };

  struct MidiSettings : InnerNode
  {
    MidiSettings(InnerNode *parent)
        : InnerNode(parent, "MIDI Settings")
    {
      children.emplace_back(new MidiReceiveSettings(this));
      children.emplace_back(new MidiSendSettings(this));
      children.emplace_back(new MidiLocalSettings(this));
      children.emplace_back(new HardwareEnableSettings(this));
      children.emplace_back(new MidiMappingSettings(this));
      children.emplace_back(new MidiProgramChangeBank(this));
      children.emplace_back(new MidiPanicButton(this));
      children.emplace_back(new ResetMidiSettingsToClassic(this));
      children.emplace_back(new ResetMidiSettingsToHighRes(this));
    }
  };

  struct FlacSettings : InnerNode
  {
    FlacSettings(InnerNode *parent)
        : InnerNode(parent, "Recorder Settings")
    {
      children.emplace_back(new EnumSettingItem<AutoStartRecorderSetting>(this, "Auto-Start Recorder"));
    }
  };

  struct Setup : InnerNode
  {
    Setup()
        : InnerNode(nullptr, "Setup")
    {
      children.emplace_back(new DeviceSettings(this));
      children.emplace_back(new HardwareUI(this));
      children.emplace_back(new MidiSettings(this));
      children.emplace_back(new FlacSettings(this));
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

bool SetupLayout::isInSelectionMode() const
{
  auto focus = m_tree->focus->get();
  auto focusEditable = dynamic_cast<const NavTree::Leaf *>(focus);
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

  if(m_focusAndMode.mode == UIMode::Select)
  {
    fb.setColor(FrameBufferColors::C179);
    fb.drawRect(Rect(0, 28, 256, 12));
  }
  return true;
}
