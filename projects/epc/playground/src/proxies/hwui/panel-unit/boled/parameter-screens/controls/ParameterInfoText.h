#pragma once

#include "MultiLineLabel.h"
#include "tools/RecursionGuard.h"
#include "Scrollable.h"

class Parameter;
class Application;
class ControlOwner;

class ParameterInfoText : public MultiLineLabel, public Scrollable
{
 private:
  typedef MultiLineLabel super;

 public:
  explicit ParameterInfoText(ControlOwner *parent);

  void setPosition(const Rect &rect) override;
  const Rect &getPosition() const override;
  void setDirty() override;

 protected:
  std::shared_ptr<Font> getFont() override;

 private:
  void onParameterChanged(const Parameter *newParameter);
  void onParameterSelected(Parameter *oldParam, Parameter *newParam);
  void onTextLoaded(const Glib::ustring &text);

  ControlOwner *m_parent;
  sigc::connection m_currentParameterConnection;
};
