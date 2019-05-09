#pragma once

#include "playground.h"

class Application;

class ParameterDescriptionDatabase
{
 public:
  static ParameterDescriptionDatabase &get();

  connection load(int paramID, slot<void, const Glib::ustring &>);

 private:
  ParameterDescriptionDatabase();

  class Job;
<<<<<<< HEAD
  typedef std::shared_ptr<Job> tJob;
  std::map<int, tJob> m_jobs;
=======
  typedef shared_ptr<Job> tJob;
  map<int, tJob> m_jobs;
>>>>>>> layouts-reloaded
};
