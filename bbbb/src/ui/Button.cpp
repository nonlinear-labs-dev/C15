#include "Button.h"
#include <io/FromButtonsBridge.h>
#include <Application.h>
#include <io/Bridges.h>

Button::Button(int buttonId, const std::string& title)
    : m_buttonId(buttonId)
{
  set_size_request(1, 1);

  if(title.empty())
    set_label(std::to_string(buttonId));
  else
    set_label(title);
}

Button::Button(const std::string& title, std::function<void()> cb)
    : m_cb(cb)
{
  set_size_request(1, 1);
  set_label(title);
}

Button::~Button()
{
}

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
}

void Button::on_released()
{
  if(!m_cb)
  {
    auto b = Application::get().getBridges()->getBridge<FromButtonsBridge>();
    b->sendKey(m_buttonId, false);
  }
}
