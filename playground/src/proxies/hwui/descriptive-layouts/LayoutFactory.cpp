#include "LayoutFactory.h"
#include "GenericLayout.h"
#include <Application.h>
#include <proxies/hwui/HWUI.h>

namespace DescriptiveLayouts
{
  namespace LayoutStyles
  {
    namespace GroupHeader
    {
      static std::initializer_list<StyleRule> bg =
      {
       { Style::Color, (int) (StyleValues::Color::C179) }
      };

      static std::initializer_list<StyleRule> text =
      {
       { Style::Color, (int) (StyleValues::Color::C43) }
      };
    }

    namespace Slider
    {
      namespace Active
      {
        static std::initializer_list<StyleRule> border =
        {
         { Style::Color, (int) (StyleValues::Color::C204) },
         { Style::BorderStyle, (int) (StyleValues::BorderStyle::Solid) }
        };

        static std::initializer_list<StyleRule> bg =
        {
         { Style::Color, (int) (StyleValues::Color::C43) }
        };

        static std::initializer_list<StyleRule> fg =
        {
         { Style::Color, (int) (StyleValues::Color::C255) }
        };
      }
    }
  }

  BoledLayoutFactory::BoledLayoutFactory()
  {
    registerTemplate("unmodulateable-parameter",
        { UIFocus::Parameters, UIMode::Select, nullptr },
        {
          { "group-bg", Components::Bar, { 0, 0, 64, 16 }, LayoutStyles::GroupHeader::bg} ,
          { "group-text", Components::Text, { 0, 0, 64, 16 }, LayoutStyles::GroupHeader::text},
          { "slider-border", Components::Border, { 64, 26, 128, 12 }, LayoutStyles::Slider::Active::border},
          { "slider-bg", Components::Bar, { 65, 27, 126, 10 }, LayoutStyles::Slider::Active::bg},
          { "slider-fg", Components::Bar, { 66, 28, 124, 8 }, LayoutStyles::Slider::Active::fg},
        },
        {
          { EventSources::parameterGroupName, "group-text", ComponentValues::text, nullptr },
          { EventSources::parameterValue, "slider-fg", ComponentValues::length, nullptr }
        },
        {
          { EventSinks::decParam, BUTTON_DEC },
          { EventSinks::incParam, BUTTON_INC }
        });
  }

  BoledLayoutFactory& BoledLayoutFactory::get()
  {
    static BoledLayoutFactory factory;
    return factory;
  }

  void BoledLayoutFactory::registerTemplate(const char* id, Selector selector,
                                            std::initializer_list<TemplateElement> elements,
                                            std::initializer_list<EventSourceMapping> sourceMapping,
                                            std::initializer_list<EventSinkMapping> sinkMappings)
  {
    m_templates.push_back( { id, selector, elements, sourceMapping, sinkMappings });
  }

  Template& BoledLayoutFactory::findTemplate(const char* name)
  {
    auto it = std::find_if(m_templates.rbegin(), m_templates.rend(), [&name](const Template &t)
    {
      return strcmp(t.id, name) == 0;
    });
    assert(it != m_templates.rend());
    return *it;
  }

  Template& BoledLayoutFactory::findTemplate(UIFocus focus, UIMode mode)
  {
    auto it = std::find_if(m_templates.rbegin(), m_templates.rend(), [=](const Template &t)
    {
      auto& selCB = t.selector.m_detail;
      return t.selector.m_focus == focus && t.selector.m_mode == mode && (!selCB || selCB());
    });

    assert(it != m_templates.rend());
    return *it;
  }

  std::shared_ptr<DFBLayout> BoledLayoutFactory::instantiate(UIFocus focus, UIMode mode)
  {
    auto temp = findTemplate(focus, mode);
    auto &boled = Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled();
    auto ret = new GenericLayout({temp}, boled);
    return std::shared_ptr<DFBLayout>(ret);
  }
}
