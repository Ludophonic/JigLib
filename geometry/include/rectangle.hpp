//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file rectangle.hpp 
//                     
//==============================================================
#ifndef JIGRECTANGLE_HPP
#define JIGRECTANGLE_HPP

namespace JigLib
{
  class tRectangle
  {
  public:
    tRectangle() {}
    tRectangle(const tVector3 & origin, const tVector3 & edge0, const tVector3 & edge1) 
      :
      mOrigin(origin), mEdge0(edge0), mEdge1(edge1) {}
    
    tVector3 GetPoint(tScalar t0, tScalar t1) const {return mOrigin + t0 * mEdge0 + t1 * mEdge1;}
    
    tVector3 mOrigin;
    tVector3 mEdge0, mEdge1;
  };
}

#endif
