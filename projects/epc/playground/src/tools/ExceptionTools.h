#include <utility>

#pragma once
#include <exception>
#include <string>
#include <stdexcept>
#include <glibmm/ustring.h>
#include <nltools/logging/Log.h>

namespace ExceptionTools
{
  inline Glib::ustring handle_eptr(const std::exception_ptr& eptr)
  {
    try
    {
      if(eptr)
      {
        std::rethrow_exception(eptr);
      }
    }
    catch(const std::out_of_range& e)
    {
      return Glib::ustring("Caught std::out_of_range ") + e.what();
    }
    catch(const std::runtime_error& e)
    {
      return Glib::ustring("Caught runtime_error ") + e.what();
    }
    catch(const std::exception& e)
    {
      return Glib::ustring("Caught exception ") + e.what();
    }
    catch(...)
    {
      return "Unknown Exception!";
    }
    return "";
  }

  static void errorLogCurrentException() {
    nltools::Log::error(handle_eptr(std::current_exception()));
  }

  class TemplateException : public std::exception
  {
   protected:
    Glib::ustring m_what, m_where;

   public:
    TemplateException(Glib::ustring what, Glib::ustring where)
        : m_what(std::move(what))
        , m_where(std::move(where))
    {
    }

    Glib::ustring what()
    {
      return m_what;
    }

    Glib::ustring where()
    {
      return m_where;
    }
  };
}
