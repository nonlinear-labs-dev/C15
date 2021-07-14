#pragma once

#include <proxies/hwui/Layout.h>
#include "SetupSelectionEntries.h"
#include "ViewEntries.h"

namespace NavTree
{
  class Setup;
}

class SetupEditor;

class SetupLayout : public Layout
{
 private:
  typedef Layout super;

 public:
  explicit SetupLayout(FocusAndMode focusAndMode);
  ~SetupLayout() override;

  bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;
  bool onRotary(int inc, ButtonModifiers modifiers) override;
  bool redraw(FrameBuffer &fb) override;
  void diveUp();

 private:
  void buildPage();
  void addSelectionEntry(Control *s);
  void addView(Control *c, bool focussed);
  void setEditor(Control *c);

  std::shared_ptr<SetupSelectionEntries> ensureSelectionEntries();
  std::shared_ptr<ViewEntries> ensureViewEntries();
  void addBreadcrumb();
  void addSelectionEntries();
  void addValueViews();
  bool addEditor();
  bool isInSelectionMode() const;
  bool isInEditMode() const;
  void finishLists();
  void onEnterInSelectionMode(ButtonModifiers modifiers);
  void onEnterInEditMode();

  std::unique_ptr<NavTree::Setup> m_tree;
  FocusAndMode m_focusAndMode;
  SetupEditor *m_editor;
};
