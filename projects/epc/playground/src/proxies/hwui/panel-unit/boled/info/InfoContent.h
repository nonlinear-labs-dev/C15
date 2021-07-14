#pragma once

#include <proxies/hwui/controls/ControlWithChildren.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/Scrollable.h>
#include <map>

class InfoField;

class InfoContent : public Scrollable, public ControlWithChildren
{
 private:
  typedef ControlWithChildren super;

 public:
  InfoContent();
  ~InfoContent() override;

  void setPosition(const Rect& rect) override;
  const Rect& getPosition() const override;
  void setDirty() override;

  virtual void fillContents() = 0;
  void updateContent();

  InfoField* addInfoField(const std::string& lineIdentifier, const Glib::ustring& labelText, Control* field);
  InfoField* addInfoField(const std::string& lineIdentifier, const Glib::ustring& labelText);

 protected:
  void fixLayout();

  std::vector<std::string> infoOrder;
  std::map<std::string, std::unique_ptr<InfoField>> infoFields;
};
