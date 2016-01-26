//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file intersection.hpp 
///
/// This covers cases where primitives are swept against each other.
/// For example, a ray-sphere test is a swept-point vs sphere test.
/// The tests are generally "sided" - i.e. if there is intersection
/// you'll get info about the first intersection, but maybe
/// not about subsequent (back-face) intersections - to get them
/// do the test with the sweep direction reversed.
///
/// The order of all these functions has the simplest primitive first
//                     
//==============================================================
#ifndef INTERSECTION_HPP
#define INTERSECTION_HPP

#include "../geometry/include/line.hpp"
#include "../geometry/include/rectangle.hpp"
#include "../geometry/include/triangle.hpp"
#include "../geometry/include/sphere.hpp"
#include "../geometry/include/capsule.hpp"
#include "../geometry/include/plane.hpp"
#include "../geometry/include/heightmap.hpp"
#include "../geometry/include/box.hpp"
#include "../geometry/include/distance.hpp"
#include "../geometry/include/aabox.hpp"

namespace JigLib
{
  /// Indicates if a segment intersects a triangle - if so the segment and triangle intersection
  /// parameters are optionally returned
  bool SegmentTriangleIntersection(tScalar* tS, tScalar* tT0, tScalar* tT1, const tSegment& seg, const tTriangle& tri);

  /// Indicates if a segment intersects a plane and if so the segment parameter is
  /// optionally returned
  bool SegmentPlaneIntersection(tScalar* tS, const tSegment& seg, const tPlane& plane);

  /// Indicates if a segment intersections a sphere. Optionally the intersection point is
  /// returned. If the segment starts inside the sphere then an intersection time of 0
  /// is returned.
  bool SegmentSphereIntersection(tScalar* tS, const tSegment& seg, const tSphere& sphere);

  /// Indicates if a segment intersections an infinitely long cylinder. Optionally the 
  /// intersection point is returned. If the segment starts inside then an intersection time of 0
  /// is returned.
  bool SegmentInfiniteCylinderIntersection(tScalar* tS, const tSegment& seg, const tSegment& cylinderAxis, tScalar radius);

  /// Indicates if a segment intersections a capsule. Optionally the intersection point is
  /// returned. If the segment starts inside the capsule then an intersection time of 0
  /// is returned.
  bool SegmentCapsuleIntersection(tScalar* tS, const tSegment& seg, const tCapsule& capsule);

  /// Indicates if a swept sphere would Intersection a plane, and if so returns the t value (between
  /// the old and new sphere positions) when intersection just occurs.
  /// Both spheres should have the same radius!!
  /// This returns false if the spheres are moving in the direction of the plane normal
  /// If you've already calculated the distances of the sphere centres to the plane, pass them in.
  /// The contact point final penetration are returned if there is Intersection
  bool SweptSpherePlaneIntersection(tVector3& pt, tScalar& finalPenetration,
                                    const tSphere& sphereOld, const tSphere& sphereNew, const tPlane& plane, 
                                    tScalar* oldCentreDistToPlane, tScalar* newCentreDistToPlane);

  /// Edges to test in SweptSphereTriangleIntersection
  enum tEdgesToTest
  {
    EDGE_NONE = 0,
    EDGE_0 = 1 << 0,
    EDGE_1 = 1 << 1,
    EDGE_2 = 1 << 2,
    EDGE_ALL = EDGE_0 | EDGE_1 | EDGE_2
  };

  /// Corners to test in SweptSphereTriangleIntersection
  enum tCornersToTest
  {
    CORNER_NONE = 0,
    CORNER_0 = 1 << 0,
    CORNER_1 = 1 << 1,
    CORNER_2 = 1 << 2,
    CORNER_ALL = CORNER_0 | CORNER_1 | CORNER_2
  };

  /// If there is collision then it returns the contact point and normal, and the
  /// approximate final penetration depth
  /// If the sphere distance to plane is known, then pass it in to help speed things up
  bool SweptSphereTriangleIntersection(tVector3& pt, tVector3& N, tScalar& depth,
                                       const tSphere& oldSphere, const tSphere& newSphere, 
                                       const tTriangle& triangle,
                                       tScalar* oldCentreDistToPlane = 0, tScalar* newCentreDistToPlane = 0,
                                       tEdgesToTest edgesToTest = EDGE_ALL,
                                       tCornersToTest cornersToTest = CORNER_ALL);

#include "../geometry/include/intersection.inl"
}

#endif
