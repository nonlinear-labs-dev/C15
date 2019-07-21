#pragma once

#include <glib.h>
#include <glibmm.h>
#include <giomm.h>
#include <sigc++/sigc++.h>
#include <sigc++/trackable.h>
#include <memory>
#include <atomic>
#include <stdio.h>
#include <iostream>

enum class Domain : uint8_t
{
  Lpc = 1,
};

void printCurrentTime(const char *s);
void printCurrentTime(const char *s, int64_t ms);
