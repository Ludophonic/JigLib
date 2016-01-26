//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file overlap.inl 
//                     
//==============================================================

//====================================================================
// SegmentPlaneOverlap
//====================================================================
inline bool SegmentPlaneOverlap(const tSegment& seg, const tPlane& plane)
{
  tScalar denom = Dot(plane.GetN(), seg.GetDelta());
  if (Abs(denom) > SCALAR_TINY)
  {
    tScalar t = -(Dot(plane.GetN(), seg.GetOrigin()) + plane.GetD()) / denom;
    if (t < 0.0f || t > 1.0f)
      return false;
    return true;
  }
  else
  {
    // parallel - return false even if it's in the plane
    return false;
  }
}

//====================================================================
// SegmentTriangleOverlap
// See Real-Time Rendering p581
//====================================================================
inline bool SegmentTriangleOverlap(const tSegment& seg, const tTriangle& triangle)
{
  /// the parameters - if hit then they get copied into the args
  tScalar u, v, t;

  const tVector3& e1 = triangle.GetEdge0();
  const tVector3& e2 = triangle.GetEdge1();
  tVector3 p = Cross(seg.GetDelta(), e2);
  tScalar a = Dot(e1, p);
  if (a > -SCALAR_TINY && a < SCALAR_TINY)
    return false;
  tScalar f = 1.0f / a;
  tVector3 s = seg.GetOrigin() - triangle.GetOrigin();
  u = f * Dot(s, p);
  if (u < 0.0f || u > 1.0f)
    return false;
  tVector3 q = Cross(s, e1);
  v = f * Dot(seg.GetDelta(), q);
  if (v < 0.0f || (u + v) > 1.0f)
    return false;
  t = f * Dot(e2, q);
  if (t < 0.0f || t > 1.0f)
    return false;
  return true;
}

//====================================================================
// SweptSpherePlaneOverlap
//====================================================================
inline bool SweptSpherePlaneOverlap(tVector3& pt, tScalar& finalPenetration,
                                    const tSphere& oldSphere, const tSphere& newSphere, 
                                    const tPlane& plane, 
                                    tScalar* pOldDistToPlane, tScalar* pNewDistToPlane)
{
  const tScalar oldDistToPlane = pOldDistToPlane ? *pOldDistToPlane : PointPlaneDistance(oldSphere.GetPos(), plane);
  const tScalar newDistToPlane = pNewDistToPlane ? *pNewDistToPlane : PointPlaneDistance(newSphere.GetPos(), plane);
  const tScalar radius = oldSphere.GetRadius();

  if (newDistToPlane >= oldDistToPlane)
    return false;
  if (newDistToPlane > radius)
    return false;

  // intersect with plane
  tScalar t = (newDistToPlane - radius) / (newDistToPlane - oldDistToPlane);
  if (t < 0.0f || t > 1.0f)
    return false;

  pt = oldSphere.GetPos() + t * (newSphere.GetPos() - oldSphere.GetPos()) - Min(radius, oldDistToPlane) * plane.GetN();
  finalPenetration = radius - newDistToPlane;
  return true;
}


//====================================================================
// SegmentAABoxOverlap
//====================================================================
inline bool SegmentAABoxOverlap(const tSegment& seg, const tAABox AABox)
{
  const tVector3& p0 = seg.GetOrigin();
  const tVector3  p1 = seg.GetEnd();
  // The AABox faces are aligned with the world directions. Loop 
  // over the 3 directions and do the two tests.
  for (unsigned iDir = 0 ; iDir < 3 ; ++iDir)
  {
    unsigned jDir = (iDir + 1) % 3;
    unsigned kDir = (iDir + 2) % 3;
    // one plane goes through the origin, one is offset
    tScalar faceOffsets[] = {AABox.GetMinPos()[iDir], AABox.GetMaxPos()[iDir]};
    for (unsigned iFace = 0 ; iFace < 2 ; ++iFace)
    {
      // distance of each point from to the face plane
      tScalar dist0 = p0[iDir] - faceOffsets[iFace];
      tScalar dist1 = p1[iDir] - faceOffsets[iFace];
      tScalar frac = -1.0f;
      if (dist0 * dist1 < -SCALAR_TINY)
        frac = -dist0 / (dist1 - dist0);
      else if (Abs(dist0) < SCALAR_TINY)
        frac = 0.0f;
      else if (Abs(dist1) < SCALAR_TINY)
        frac = 1.0f;
      if (frac >= 0.0f)
      {
        Assert(frac <= 1.0f);
        tVector3 pt = seg.GetPoint(frac);
        // check the point is within the face rectangle
        if ((pt[jDir] > AABox.GetMinPos()[jDir] - SCALAR_TINY) && 
            (pt[jDir] < AABox.GetMaxPos()[jDir] + SCALAR_TINY) && 
            (pt[kDir] > AABox.GetMinPos()[kDir] - SCALAR_TINY) && 
            (pt[kDir] < AABox.GetMaxPos()[kDir] + SCALAR_TINY) )
        {
          return true;
        }
      }
    }
  }
  return false;
}

