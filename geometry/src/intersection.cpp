//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file intersection.cpp 
//                     
//==============================================================
#include "intersection.hpp"
#include "distance.hpp"
#include <limits>
using namespace JigLib;

//====================================================================
// SweptSphereTriangleIntersection
// See Real Time Rendering p624
//====================================================================
bool JigLib::SweptSphereTriangleIntersection(tVector3& pt, tVector3& N, tScalar& depth,
                                             const tSphere& oldSphere, const tSphere& newSphere, 
                                             const tTriangle& triangle,
                                             tScalar* oldCentreDistToPlane, tScalar* newCentreDistToPlane, 
                                             tEdgesToTest edgesToTest,
                                             tCornersToTest cornersToTest)
{
  unsigned i;
  const tPlane trianglePlane(triangle.GetPlane());

  // Check against plane
  if (!SweptSpherePlaneIntersection(pt, depth, oldSphere, newSphere, trianglePlane, oldCentreDistToPlane, newCentreDistToPlane))
    return false;

  tVector3 trianglePts[3] = {
    triangle.GetPoint(0), 
    triangle.GetPoint(1), 
    triangle.GetPoint(2) };
  tVector3 triangleEdges[3] = {
    trianglePts[1] - trianglePts[0], 
    trianglePts[2] - trianglePts[1], 
    trianglePts[0] - trianglePts[2] };

  // If the point is inside the triangle, this is a hit
  bool allInside = true;
  for (i = 0 ; i < 3 ; ++i)
  {
    tVector3 outDir = Cross(triangleEdges[i], trianglePlane.GetN());
    if (Dot(pt - trianglePts[i], outDir) > 0.0f)
    {
      allInside = false;
      break;
    }
  }

  // Quick result?
  if (allInside)
  {
    N = trianglePlane.GetN();
    return true;
  }

  // Now check against the edges
  tScalar bestT = SCALAR_HUGE;
  const tVector3 Ks = newSphere.GetPos() - oldSphere.GetPos();
  const tScalar kss = Dot(Ks, Ks);
  const tScalar radius = newSphere.GetRadius();
  const tScalar radiusSq = Sq(radius);
  for (i = 0 ; i < 3 ; ++i)
  {
    unsigned mask = 1 << i;
    if (!(mask & edgesToTest))
      continue;
    const tVector3& Ke = triangleEdges[i];
    const tVector3  Kg = trianglePts[i] - oldSphere.GetPos();

    const tScalar kee = Dot(Ke, Ke);
    if (Abs(kee) < SCALAR_TINY)
      continue;
    const tScalar kes = Dot(Ke, Ks);
    const tScalar kgs = Dot(Kg, Ks);
    const tScalar keg = Dot(Ke, Kg);
    const tScalar kgg = Dot(Kg, Kg);

    // a * t^2 + b * t + c = 0
    const tScalar a = kee * kss - Sq(kes);
    if (Abs(a) < SCALAR_TINY)
      continue;
    const tScalar b = 2.0f * (keg * kes - kee * kgs);
    const tScalar c = kee * (kgg - radiusSq) - Sq(keg);
  
    tScalar blah = Sq(b) - 4.0f * a * c;
    if (blah < 0.0f)
      continue;

    // solve for t - take minimum
    const tScalar t = (-b - Sqrt(blah)) / (2.0f * a);

    if (t < 0.0f || t > 1.0f)
      continue;

    if (t > bestT)
      continue;

    // now check where it hit on the edge
    tVector3 Ct = oldSphere.GetPos() + t * Ks;
    tScalar d = Dot((Ct - trianglePts[i]), triangleEdges[i]) / kee;

    if (d < 0.0f || d > 1.0f)
      continue;

    // wahay - got hit. Already checked that t < bestT
    bestT = t;

    pt = trianglePts[i] + d * triangleEdges[i];
    N = (Ct - pt).GetNormalisedSafe();
    // depth is already calculated
  }
  if (bestT <= 1.0f)
    return true;

  // check the corners
  bestT = SCALAR_HUGE;
  for (i = 0 ; i < 3 ; ++i)
  {
    unsigned mask = 1 << i;
    if (!(mask & cornersToTest))
      continue;
    const tVector3  Kg = trianglePts[i] - oldSphere.GetPos();
    const tScalar kgs = Dot(Kg, Ks);
    const tScalar kgg = Dot(Kg, Kg);
    tScalar a = kss;
    if (Abs(a) < SCALAR_TINY)
      continue;
    tScalar b = -2.0f * kgs;
    tScalar c = kgg - radiusSq;

    tScalar blah = Sq(b) - 4.0f * a * c;
    if (blah < 0.0f)
      continue;

    // solve for t - take minimum
    const tScalar t = (-b - Sqrt(blah)) / (2.0f * a);

    if (t < 0.0f || t > 1.0f)
      continue;

    if (t > bestT)
      continue;

    bestT = t;

    pt = trianglePts[i];
    tVector3 Ct = oldSphere.GetPos() + t * Ks;
    N = (Ct - pt).GetNormalisedSafe();
  }
  if (bestT <= 1.0f)
    return true;

  return false;
}

