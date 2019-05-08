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
  typedef shared_ptr<Job> tJob;
  map<int, tJob> m_jobs;
};
