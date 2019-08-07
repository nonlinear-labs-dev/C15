#pragma once

#include <proxies/hwui/controls/LeftAlignedLabel.h>
#include "MenuEditor.h"

class DisplayEditor : public ControlWithChildren, public SetupEditor
{
 public:
  DisplayEditor();

  bool redraw(FrameBuffer& fb) override;

  void drawBackground(FrameBuffer& fb) override;

 protected:
  void doLayout();
  std::pair<LeftAlignedLabel*, LeftAlignedLabel*> addEntry(const Glib::ustring& key, const Glib::ustring& name,
                                                           const Glib::ustring& value);
  std::map<Glib::ustring, std::pair<LeftAlignedLabel*, LeftAlignedLabel*>> m_entrys;
  std::vector<Glib::ustring> m_order;
};