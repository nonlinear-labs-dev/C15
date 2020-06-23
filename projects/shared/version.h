#pragma once

#ifndef NO_AUTOCONF
#include "build-info.h"
#else
#include "build-info.h.in"
#warning "build-info.h has not been auto-configured. Using build-info.h.in"
#endif

  
