#pragma once

#include <glibmm.h>
#include <giomm.h>
#include <thread>

class CommandLineInterface
{
 public:
  CommandLineInterface();
  virtual ~CommandLineInterface();

 protected:
  virtual void processByte(char c) = 0;

 private:
  void background();
  void onKeyboardByteRead(Glib::RefPtr<Gio::AsyncResult> &res);
  bool onIO();

  std::thread m_bg;
  bool m_close = false;
};
