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
  ParameterInfoText(ControlOwner *parent);

  virtual void setPosition(const Rect &rect) override;
  virtual const Rect &getPosition() const override;
  virtual void setDirty() override;

 protected:
  virtual Oleds::tFont getFont() override;

 private:
  void loadInfoText(Parameter *oldParam, Parameter *newParam);
  void onTextLoaded(const Glib::ustring &text);

  ControlOwner *m_parent;
  RecursionGuard m_dirtyGuard;
  connection m_connection;
};
