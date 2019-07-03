#include <utility>

#pragma once
#include <exception>
#include <string>
#include <stdexcept>
#include <glibmm/ustring.h>

namespace ExceptionTools
{
  inline Glib::ustring handle_eptr(std::exception_ptr eptr)
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

  class TemplateException : public std::exception
  {
   protected:
    using ustring = Glib::ustring;
    ustring m_what, m_where;

   public:
    TemplateException(ustring what, ustring where)
        : m_what(std::move(what))
        , m_where(std::move(where))
    {
    }

    ustring what()
    {
      return m_what;
    }

    ustring where()
    {
      return m_where;
    }
  };
}