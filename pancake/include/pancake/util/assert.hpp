#pragma once

#include "util/fewi.hpp"

#ifdef NDEBUG
#define ensure(condition) ((void)0)
#define ensureLoc(condition, location) ((void)0)
#else
#define ensure(condition) \
  if (!(condition))       \
    pancake::FEWI::error(#condition " is false!");
#define ensureLoc(condition, location) \
  if (!(condition))                    \
    pancake::FEWI::error(#condition " is false!", location);
#endif