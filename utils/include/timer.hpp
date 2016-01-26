//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file timer.hpp 
//                     
//==============================================================
#include "../utils/include/time.hpp"

#ifdef WIN32
#include <windows.h>
#endif

namespace JigLib
{
#ifdef WIN32
  typedef LARGE_INTEGER tHighResTimeVal;
#else
  typedef int tHighResTimeVal;
#endif
  // convert time delta into useable values
  tTime GetTimeDelta(tHighResTimeVal t0, tHighResTimeVal t1);
  
  /// available indicates if a high-resolution time was available. If
  /// not then don't believe what this returns.
  tHighResTimeVal GetHighResTime(bool & available);
  
}
