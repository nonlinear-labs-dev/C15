#include <nltools/Assert.h>

#include <utility>
#include "InfoField.h"
#include "SingleLineInfoContent.h"
#include "MultiLineInfoContent.h"

InfoField::InfoField(SingleLineInfoContent* l, Control* c)
    : m_label(l)
    , m_content(c)
{
}

void InfoField::setInfo(const Glib::ustring& text, FrameBufferColors c)
{
  if(auto label = dynamic_cast<SingleLineInfoContent*>(m_content))
  {
    label->setFontColor(c);
    label->setText(StringAndSuffix { text });
  }
  else if(auto multiLineLabel = dynamic_cast<MultiLineInfoContent*>(m_content))
  {
    multiLineLabel->setText(text, c);
  }
  else
  {
    nltools_assertAlways(false);
  }
}

void InfoField::setInfo(const Glib::ustring& text)
{
  setInfo(text, FrameBufferColors::C128);
}

int InfoField::format(int y)
{
  setPosition(y);
  auto height = std::max(m_content->getHeight(), m_label->getHeight());
  return y + height;
}

void InfoField::setPosition(int y)
{
  auto height = std::max(m_content->getHeight(), 16);
  m_label->setPosition(Rect(0, y, 64, 16));
  if(auto label = dynamic_cast<SingleLineInfoContent*>(m_content))
  {
    label->setPosition(Rect(64, y, 256 - 64, height));
  }
  else if(auto multiLineLabel = dynamic_cast<MultiLineInfoContent*>(m_content))
  {
    multiLineLabel->setPosition(Rect(64, y, 256 - 64, height));
  }
  else
  {
    nltools_assertAlways(false);
  }
}
