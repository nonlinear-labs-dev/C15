#include "Button.h"
#include <io/FromButtonsBridge.h>
#include <Application.h>
#include <io/Bridges.h>

#include <utility>

Button::Button(int buttonId, const std::string& title)
    : m_buttonId(buttonId)
    , m_cssProvider { Gtk::CssProvider::create() }
{
  set_size_request(1, 1);

  if(title.empty())
    set_label(std::to_string(buttonId));
  else
    set_label(title);

  get_style_context()->add_provider(m_cssProvider, -1);
  m_cssProvider->load_from_data(".pressed {background-image: none; background-color: red; }");
}

Button::Button(const std::string& title, std::function<void()> cb)
    : m_cb(std::move(cb))
    , m_cssProvider { Gtk::CssProvider::create() }
{
  set_size_request(1, 1);
  set_label(title);

  get_style_context()->add_provider(m_cssProvider, -1);
  m_cssProvider->load_from_data(".pressed {background-image: none; background-color: red; }");
}

Button::~Button() = default;

void Button::setLed(int idx, bool state)
{
  if(m_buttonId == idx)
  {
    if(m_state != state)
    {
      m_state = state;
      queue_draw();
    }
  }
}

bool Button::on_draw(const ::Cairo::RefPtr<::Cairo::Context>& cr)
{
  Gtk::Button::on_draw(cr);

  if(m_state)
  {
    cr->begin_new_path();
    auto x = get_allocated_width() / 2;
    auto y = get_allocated_height() / 2;
    auto radius = std::min(x, y) / 2;
    cr->arc(x, y, radius, 0, 2 * M_PI);
    cr->set_source_rgb(1, 0, 0);
    cr->set_line_width(0.5);
    cr->close_path();
    cr->fill_preserve();
    cr->set_source_rgb(0.2, 0.2, 0.2);
    cr->stroke();
  }

  return true;
}

void Button::on_pressed()
{
  if(m_cb)
  {
    m_cb();
  }
  else
  {
    auto b = Application::get().getBridges()->getBridge<FromButtonsBridge>();
    b->sendKey(m_buttonId, true);
  }

  get_style_context()->add_class("pressed");
  m_pressed = true;
}

bool Button::on_button_press_event(GdkEventButton* button_event)
{
  if(button_event->button == 3 && button_event->type == GDK_BUTTON_PRESS)  // 3 == Right Mouse
  {
    if(!m_pressed)
    {
      on_pressed();
    }
    else
    {
      on_released();
    }
  }
  return Widget::on_button_press_event(button_event);
}

void Button::on_released()
{
  if(!m_cb)
  {
    auto b = Application::get().getBridges()->getBridge<FromButtonsBridge>();
    b->sendKey(m_buttonId, false);
  }

  set_use_underline(false);
  get_style_context()->remove_class("pressed");
  m_pressed = false;
}
