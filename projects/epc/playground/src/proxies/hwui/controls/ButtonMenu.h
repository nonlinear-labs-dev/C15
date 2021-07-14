#pragma once

#include "ControlWithChildren.h"
#include <proxies/hwui/Font.h>

class Application;

class ButtonMenu : public ControlWithChildren
{
 private:
  typedef ControlWithChildren super;

 public:
  explicit ButtonMenu(const Rect &rect, size_t numButtonPlaces = 5);
  ~ButtonMenu() override;

  using Action = std::function<void()>;

  size_t addButton(const Glib::ustring &caption, Action action);
  virtual void selectButton(size_t i);
  size_t getSelectedButton() const;
  void setHighlight(bool isHighlight) override;
  void highlightSelectedButton();
  void toggle();
  void antiToggle();
  void sanitizeIndex();
  size_t sanitizeIndex(size_t index);

 protected:
  void clear() override;

 public:
  void doAction();
  const size_t getItemCount() const;

 protected:
  void setItemTitle(size_t i, const Glib::ustring &caption);

  virtual Font::Justification getDefaultButtonJustification() const;

  virtual void bruteForce();

 private:
  void clearActions();
  size_t getItemToShowAtPlace(size_t place) const;

  struct Item
  {
    Glib::ustring title;
    Action action;
  };

  std::vector<Item> m_items;
  size_t m_selected;
  size_t m_numButtonPlaces;
  int entryWidth;

  friend class SoundMenu;
};
