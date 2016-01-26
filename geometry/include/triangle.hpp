//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file triangle.hpp 
//                     
//==============================================================
#ifndef JIGTRIANGLE_HPP
#define JIGTRIANGLE_HPP

#include "../maths/include/vector3.hpp"
#include "../geometry/include/plane.hpp"

namespace JigLib
{
  /// Defines a 3d triangle. Each edge goes from the origin.
  /// Cross(edge0, edge1)  gives the triangle normal.
  class tTriangle
  {
  public:
    tTriangle();
    /// Points specified so that pt1-pt0 is edge0 and p2-pt0
    /// is edge1
    tTriangle(const tVector3& pt0, const tVector3& pt1, const tVector3& pt2);

    const tVector3 & GetOrigin() const {return mOrigin;}
    void SetOrigin(const tVector3 & origin) {mOrigin = origin;}

    const tVector3 & GetEdge0() const {return mEdge0;}
    void SetEdge0(const tVector3 & edge0) {mEdge0 = edge0;}

    const tVector3 & GetEdge1() const {return mEdge1;}
    void SetEdge1(const tVector3 & edge1) {mEdge1 = edge1;}

    /// Edge2 goes from pt1 to pt2
    tVector3 GetEdge2() const {return mEdge1 - mEdge0;}

    /// Same numbering as in the constructor
    tVector3 GetPoint(unsigned i) const;

    /// Gets the triangle normal. If degenerate it will be normalised, but
    /// the direction may be wrong!
    tVector3 GetNormal() const {return Cross(mEdge0, mEdge1).NormaliseSafe();}

    /// Gets the plane containing the triangle
    tPlane GetPlane() const {return tPlane(GetNormal(), mOrigin);}

    /// Returns the point parameterised by t0 and t1
    tVector3 GetPoint(tScalar t0, tScalar t1) const {return mOrigin + t0 * mEdge0 + t1 * mEdge1;}

    tVector3 GetCentre() const {return mOrigin + 0.333333333333f * (mEdge0 + mEdge1);}

    /// Gets the minimum and maximum extents of the triangle along the
    /// axis
    void GetSpan(tScalar & min, tScalar & max, const tVector3 & axis) const;
  private:
    tVector3 mOrigin;
    tVector3 mEdge0;
    tVector3 mEdge1;
  };

  inline tVector3 tTriangle::GetPoint(unsigned i) const
  {
    switch (i)
    {
    case 1: return mOrigin + mEdge0;
    case 2: return mOrigin + mEdge1;
    default: return mOrigin;
    }
  }

  inline tTriangle::tTriangle(const tVector3& pt0, const tVector3& pt1, const tVector3& pt2)
    : mOrigin(pt0), mEdge0(pt1 - pt0), mEdge1(pt2 - pt0) 
  {}


  //==============================================================
  // GetSpan
  //==============================================================
  inline void tTriangle::GetSpan(tScalar & min, tScalar & max, const tVector3 & axis) const
  {
    tScalar d0 = Dot(GetPoint(0), axis);
    tScalar d1 = Dot(GetPoint(1), axis);
    tScalar d2 = Dot(GetPoint(2), axis);
    min = Min(d0, d1, d2);
    max = Max(d0, d1, d2);
  }
}

#endif
