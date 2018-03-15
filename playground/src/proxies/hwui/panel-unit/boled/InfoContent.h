#pragma once

#include <proxies/hwui/controls/ControlWithChildren.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/Scrollable.h>
#include <proxies/hwui/controls/Label.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/MultiLineLabel.h>
#include <proxies/hwui/controls/LeftAlignedLabel.h>
#include <cassert>

class InfoContent : public Scrollable, public ControlWithChildren
{
  private:
    typedef ControlWithChildren super;

  public:
    struct MultiLineContent : public MultiLineLabel
    {
        MultiLineContent();
        virtual void setPosition(Rect rect);
    };

    struct SingleLineContent : public LeftAlignedLabel
    {
        SingleLineContent(Glib::ustring name);
        SingleLineContent();
    };

    struct InfoField
    {
        InfoField(SingleLineContent* label, Control* c);

        void setInfo(Glib::ustring text);
        void setInfo(Glib::ustring text, FrameBuffer::Colors c);
        void setPosition(int y);
        int format(int y);

      private:
        SingleLineContent* m_label;
        Control* m_content;
    };

    InfoContent();
    virtual ~InfoContent();

    virtual void setPosition(const Rect &rect) override;
    virtual const Rect &getPosition() const override;
    virtual void setDirty() override;

    InfoField* addInfoField(std::string lineIdentifier, Glib::ustring labelText, Control* field);
    InfoField* addInfoField(std::string lineIdentifier, Glib::ustring labelText);

  protected:
    std::map<std::string, std::unique_ptr<InfoField>> infoFields;
};

