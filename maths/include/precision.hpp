//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file precision.hpp 
//                     
//==============================================================
#ifndef JIGPRECISION_HPP
#define JIGPRECISION_HPP

#include "../include/jiglibconfig.hpp"

#define SCALAR(val) ((tScalar) val)

namespace JigLib
{
#ifdef USING_DOUBLE
  typedef double tScalar;
#else
  typedef float tScalar;
#endif
}

#ifdef OPT

#include <xmmintrin.h>
#define PREFETCH(ptr) _mm_prefetch((char*)(ptr),_MM_HINT_T0);
#define PREFETCH_LOOP(num, ptr) { for (int prefetchLoop = 0; prefetchLoop < num; prefetchLoop+=64) {_mm_prefetch( (const char*)(ptr)+prefetchLoop,_MM_HINT_T0); } }

#else

#define PREFETCH(ptr)
#define PREFETCH_Loop(num, ptr)

#endif


#endif
