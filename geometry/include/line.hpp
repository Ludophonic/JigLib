//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file tLine.hpp 
//                     
//==============================================================
#ifndef JIGLINE_HPP
#define JIGLINE_HPP
#include "../maths/include/precision.hpp"
#include "../maths/include/vector3.hpp"

namespace JigLib
{
  /// A line goes through pos, and extends infinitely far in both
  /// directions along dir.
  class tLine
  {
  public:
    tLine() {}
    tLine(const tVector3 & origin, const tVector3 & dir) : mOrigin(origin), mDir(dir) {}
    
    tVector3 GetOrigin(tScalar t) const {return mOrigin + t * mDir;}
    
    tVector3 mOrigin;
    tVector3 mDir;
  };
  
  /// A Ray is just a line that extends in the +ve direction
  class tRay
  {
  public:
    tRay() {}
    tRay(const tVector3 & origin, const tVector3 & dir) : mOrigin(origin), mDir(dir) {}
    
    tVector3 GetOrigin(tScalar t) const {return mOrigin + t * mDir;}
    
    tVector3 mOrigin;
    tVector3 mDir;
  };
  
  /// A Segment is a line that starts at origin and goes only as far as
  /// (origin + delta).
  class tSegment
  {
  public:
    tSegment() {}
    tSegment(const tVector3 & origin, const tVector3 & delta) : mOrigin(origin), mDelta(delta) {}
    
    const tVector3& GetOrigin() const {return mOrigin;}
    const tVector3& GetDelta() const {return mDelta;}

    tVector3 GetPoint(tScalar t) const {return mOrigin + t * mDelta;}
    tVector3 GetEnd() const {return mOrigin + mDelta;}
    
    tVector3 mOrigin;
    tVector3 mDelta;
  };
  
}

#endif
