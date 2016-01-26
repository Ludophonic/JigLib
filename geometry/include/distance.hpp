//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file distance.hpp 
///
/// Provides various functions to get the distances basic primitives
/// when the result cannot have a penetration depth. Also look in 
/// overlap.hpp or intersection.hpp
/// 
/// The order of all these functions has the simplest primitive first
//                     
//==============================================================
#ifndef DISTANCE_HPP
#define DISTANCE_HPP

#include "../geometry/include/line.hpp"
#include "../geometry/include/rectangle.hpp"
#include "../geometry/include/triangle.hpp"
#include "../geometry/include/sphere.hpp"
#include "../geometry/include/capsule.hpp"
#include "../geometry/include/plane.hpp"
#include "../geometry/include/heightmap.hpp"
#include "../geometry/include/box.hpp"
#include "../geometry/include/aabox.hpp"
#include "../utils/include/fixedvector.hpp"

namespace JigLib
{
  /// Point to point
  tScalar PointPointDistanceSq(const tVector3 & pt1, const tVector3 & pt2);
  tScalar PointPointDistance(const tVector3 & pt1, const tVector3 & pt2);

  /// Segment to point. Optionally returns the segment t-value that is closest
  tScalar PointSegmentDistanceSq(tScalar * t, const tVector3 & pt, const tSegment & seg);
  tScalar PointSegmentDistance(tScalar * t, const tVector3 & pt, const tSegment & seg);

  /// point to plane (+ve if above the plane, negative if below). 
  tScalar PointPlaneDistance(const tVector3 & pt, const tPlane & plane);

  /// Point to rectangle. Optionally returns the t values on the rectangle
  tScalar PointRectangleDistanceSq(tScalar * t0, tScalar * t1, const tVector3 & pt, const tRectangle & rect);

  /// Point to triangle. Optionally returns the t values on the triangle
  tScalar PointTriangleDistanceSq(tScalar * t0, tScalar * t1, const tVector3 & pt, const tTriangle & triangle);

  /// Segment to triangle. Optionally returns the segment and triangle t values of the closest point
  tScalar SegmentTriangleDistanceSq(tScalar* segT, tScalar* triT0, tScalar* triT1, const tSegment& seg, const tTriangle& triangle);

  /// Segment to segment. Optionally returns the two segment t-values that give the closest
  /// points
  tScalar SegmentSegmentDistanceSq(tScalar *t0, tScalar *t1, const tSegment & seg0, const tSegment & seg1);
  tScalar SegmentSegmentDistance(tScalar *t0, tScalar *t1, const tSegment & seg0, const tSegment & seg1);

  /// Segment to rectangle. Optionally returns the rectangle t values that give
  /// the closest point on the rectangle and the segment t value
  tScalar SegmentRectDistanceSq(tScalar * segT, tScalar * rectT0, tScalar * rectT1, const tSegment & seg, const tRectangle & rect);
  tScalar SegmentRectDistance(tScalar * segT, tScalar * rectT0, tScalar * rectT1, const tSegment & seg, const tRectangle & rect);

  /// Segment to box. Optionally returns the box t values and the segment t galue that
  /// give the closest points.
  /// \TODO currently the box t values are based around the box centre, and aren't normalised
  tScalar SegmentBoxDistanceSq(tScalar * segT, tScalar * boxT0, tScalar * boxT1, tScalar * boxT2,
                               const tSegment & seg, const tBox & box);

#include "../geometry/include/distance.inl"
}

#endif
