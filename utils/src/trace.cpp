// JigLib - Copyright (C) 2004 Danny Chapman
//
// \file trace.cpp

#include "trace.hpp"
#include <stdio.h>

namespace JigLib
{
  
  
/// Overall trace enabled
  bool traceEnabled = false;
  
/// The overall trace level - only trace with a level equal to or less
/// than this comes out.
  int traceLevel = 0;
  
/// The strings for which trace is enabled. Normally these will be
/// file names, though they don't have to be. 
  std::vector<std::string> traceStrings;
  
/// If this flag is set, all trace strings are enabled
  bool traceAllStrings = false;
  
  void TracePrintf(const char *fmt, ...)
  {
    va_list ap;
    
    // prepare log file
    static bool init = false;
    static FILE * logFile = 0;
    
    if (init == false)
    {
      init = true;
      logFile = fopen("program.log", "w");
      
      if (logFile == NULL)
      {
        fprintf(stderr, "Unable to open program.log\n");
        // We have a backup plan!! Assume that non-win32 is unix based.
#ifdef WIN32
        fprintf(stderr, "Trying C:\\program.log\n");
        char logFileName[] = "C:\\program.log";
#else
        fprintf(stderr, "Trying /tmp/program.log\n");
        char logFileName[] = "/tmp/program.log";
#endif
        logFile = fopen(logFileName, "w");
        if (logFile == NULL)
        {
          fprintf(stderr, "Unable to open backup %s\n", logFileName);
        }
        else
        {
          printf("Opened log file: %s\n", logFileName);
        }
      }
      else
      {
        printf("Opened log file: program.log\n");
      }
    }
    
    // first to stdout
    va_start(ap, fmt);
    vprintf(fmt,ap);
    va_end(ap);
    
    // now to file
    if (logFile)
    {
      va_start(ap, fmt);
      vfprintf(logFile, fmt, ap);
      // flush it line-by-line so we don't miss any
      fflush(logFile);
      va_end(ap);
    }
  }
}
