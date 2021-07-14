#pragma once

#include <nltools/StringTools.h>
#include <glibmm/spawn.h>

namespace nltools
{
  inline std::string handle_eptr(std::exception_ptr eptr)
  {
    try
    {
      if(eptr)
      {
        std::rethrow_exception(eptr);
      }
    }
    catch(const Glib::SpawnError &e)
    {
      return std::string("Caught Glib::SpawnError: " + e.what());
    }
    catch(const std::out_of_range &e)
    {
      return std::string("Caught std::out_of_range ") + e.what();
    }
    catch(const std::runtime_error &e)
    {
      return std::string("Caught runtime_error ") + e.what();
    }
    catch(const std::exception &e)
    {
      return std::string("Caught exception ") + e.what();
    }
    catch(...)
    {
      return "Unknown Exception!";
    }
    return "";
  }

  template <typename... tArgs> void throwException(const tArgs &... args)
  {
    throw std::runtime_error(nltools::string::concat(args...));
  }
}
