//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file timer.cpp 
//                     
//==============================================================
#include "timer.hpp"
#include "trace.hpp"

#ifdef WIN32
#include <windows.h>
#include <wincon.h>
#endif

using namespace JigLib;

tTime JigLib::GetTimeDelta(tHighResTimeVal t0, tHighResTimeVal t1)
{
#ifdef WIN32
  static LARGE_INTEGER frequency;
  static int use_perf = QueryPerformanceFrequency(&frequency);
  return (tTime) ((double) (t1.QuadPart - t0.QuadPart) / (double) frequency.QuadPart);
#else
  return tTime(0);
#endif
}

tHighResTimeVal JigLib::GetHighResTime(bool & available)
{
  available = false;
#ifdef WIN32
  LARGE_INTEGER currentTime;
  static LARGE_INTEGER frequency;
  static int use_perf = QueryPerformanceFrequency(&frequency);
  static bool init = false;
  if (init == false)
  {
    init = true;
    if (use_perf)
      TRACE("Using QueryPerformance for timing\n");
    else
      TRACE("QueryPerformance not available\n");
  }
  if (use_perf)
  {
    available = true;
    QueryPerformanceCounter(&currentTime);
  }
  else
  {
    currentTime.QuadPart = 0;
  }
  return currentTime;
#else
  return tHighResTimeVal(0);
#endif
}
