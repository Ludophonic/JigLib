//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file overlap.hpp 
///
/// Overlap tests compare two static geometries and tell you if they 
/// overlap. If so then you might get some information about the
/// overlap properties, but when the function mirrors an intersection
/// test it will do less (to be faster)
///
/// The order of all these functions has the simplest primitive first
//                     
//==============================================================
#ifndef OVERLAP_HPP
#define OVERLAP_HPP

#include "../geometry/include/line.hpp"
#include "../geometry/include/rectangle.hpp"
#include "../geometry/include/triangle.hpp"
#include "../geometry/include/sphere.hpp"
#include "../geometry/include/capsule.hpp"
#include "../geometry/include/plane.hpp"
#include "../geometry/include/heightmap.hpp"
#include "../geometry/include/box.hpp"
#include "../geometry/include/aabox.hpp"
#include "../geometry/include/distance.hpp"
#include "../utils/include/fixedvector.hpp"

namespace JigLib
{
  /// Indicates if a segment intersects a triangle
  bool SegmentTriangleOverlap(const tSegment& seg, const tTriangle& tri);

  /// Indicates if a segment intersects a plane
  bool SegmentPlaneOverlap(const tSegment& seg, const tPlane& plane);

  /// Indicates if a swept sphere would overlap a plane, and if so returns the t value (between
  /// the old and new sphere positions) when intersection just occurs.
  /// Both spheres should have the same radius!!
  /// This returns false if the spheres are moving in the direction of the plane normal
  /// If you've already calculated the distances of the sphere centres to the plane, pass them in.
  /// The contact point final penetration are returned if there is overlap
  bool SweptSpherePlaneOverlap(tVector3& pt, tScalar& finalPenetration,
                               const tSphere& sphereOld, const tSphere& sphereNew, const tPlane& plane, 
                               tScalar* oldCentreDistToPlane, tScalar* newCentreDistToPlane);


  /// Indicates if a segment overlaps an AABox
  bool SegmentAABoxOverlap(const tSegment& seg, const tAABox AABox);

#include "../geometry/include/overlap.inl"
}

#endif
