// JigLib - Copyright (C) 2004 Danny Chapman
#ifndef JIGASSERT_HPP
#define JIGASSERT_HPP

#include "../include/jiglibconfig.hpp"

#ifndef _DEBUG
namespace JigLib
{
#define Assert(exp)
#define AssertMsg(exp, msg)
}

#else

#include "../utils/include/trace.hpp"
#include <stdio.h>
#include <stdlib.h>

void AssertFn();

namespace JigLib
{
  
#define DO_JIG_DEATH \
printf(\
"This program has crashed\n" \
"Press 'c <RET>' to continue (bad idea!) or 'q <RET>' to exit.\n"); \
for (;;) {int c = getc(stdin); if (c == 'c') break; else if (c == 'q') abort();}
#define Assert(exp) \
if (!(exp)) { \
TRACE("Assert %s at %s:%d\n", #exp, __FILE__, __LINE__); \
AssertFn(); \
DO_JIG_DEATH \
}
  
#define AssertMsg(exp, msg) \
if (!(exp)) { \
TRACE("Assert %s at %s:%d\n%s\n", #exp, __FILE__, __LINE__, msg); \
AssertFn(); \
DO_JIG_DEATH \
}
}

#endif

#endif
