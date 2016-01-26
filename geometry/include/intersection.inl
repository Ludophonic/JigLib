//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file intersection.inl 
//                     
//==============================================================

//====================================================================
// SegmentPlaneIntersection
//====================================================================
inline bool SegmentPlaneIntersection(tScalar* tS, const tSegment& seg, const tPlane& plane)
{
  tScalar denom = Dot(plane.GetN(), seg.GetDelta());
  if (Abs(denom) > SCALAR_TINY)
  {
    tScalar t = -(Dot(plane.GetN(), seg.GetOrigin()) + plane.GetD()) / denom;
    if (t < 0.0f || t > 1.0f)
      return false;
    if (tS) *tS = t;
    return true;
  }
  else
  {
    // parallel - return false even if it's in the plane
    return false;
  }
}

//====================================================================
// SegmentTriangleIntersection
// See Real-Time Rendering p581
//====================================================================
inline bool SegmentTriangleIntersection(tScalar* tS, tScalar* tT0, tScalar* tT1, 
                                        const tSegment& seg, const tTriangle& triangle)
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
  if (tS) *tS = t;
  if (tT0) *tT0 = u;
  if (tT1) *tT1 = v;
  return true;
}

//==============================================================
// SegmentSphereIntersection
//==============================================================
inline bool SegmentSphereIntersection(tScalar* tS, const tSegment& seg, const tSphere& sphere)
{
  const tVector3& r = seg.GetDelta();
  const tVector3 s = seg.GetOrigin() - sphere.GetPos();
  tScalar radiusSq = Sq(sphere.GetRadius());
  tScalar rSq = r.GetLengthSq();
  if (rSq < radiusSq)
  {
    if (tS) *tS = 0.0f; // starting inside
    return true;
  }

  tScalar sDotr = Dot(s, r);
  tScalar sSq = s.GetLengthSq();
  tScalar sigma = Sq(sDotr) - rSq * (sSq - radiusSq);
  if (sigma < 0.0f)
    return false;
  tScalar sigmaSqrt = Sqrt(sigma);
  tScalar lambda1 = (-sDotr - sigmaSqrt) / rSq;
  tScalar lambda2 = (-sDotr + sigmaSqrt) / rSq;
  if (lambda1 > 1.0f || lambda2 < 0.0f)
    return false;
  // intersection!
  if (tS) *tS = Max(lambda1, 0.0f);
  return true;
}

//==============================================================
// SegmentInfiniteCylinderIntersection
//==============================================================
inline bool SegmentInfiniteCylinderIntersection(tScalar* tS, const tSegment& seg, const tSegment& cylinderAxis, tScalar radius)
{
  const tVector3& Ks = seg.GetDelta();
  const tScalar kss = Dot(Ks, Ks);
  const tScalar radiusSq = Sq(radius);

  const tVector3& Ke = cylinderAxis.GetDelta();
  const tVector3  Kg = cylinderAxis.GetOrigin() - seg.GetOrigin();

  const tScalar kee = Dot(Ke, Ke);
  if (Abs(kee) < SCALAR_TINY)
    return false;

  const tScalar kes = Dot(Ke, Ks);
  const tScalar kgs = Dot(Kg, Ks);
  const tScalar keg = Dot(Ke, Kg);
  const tScalar kgg = Dot(Kg, Kg);

  // check if start is inside
  tScalar distSq = (Kg - (keg * Ke) / kee).GetLengthSq();
  if (distSq < radiusSq)
  {
    if (tS) *tS = 0.0f;
    return true;
  }

  // a * t^2 + b * t + c = 0
  const tScalar a = kee * kss - Sq(kes);
  if (Abs(a) < SCALAR_TINY)
    return false;
  const tScalar b = 2.0f * (keg * kes - kee * kgs);
  const tScalar c = kee * (kgg - radiusSq) - Sq(keg);

  tScalar blah = Sq(b) - 4.0f * a * c;
  if (blah < 0.0f)
    return false;

  // solve for t - take minimum
  const tScalar t = (-b - Sqrt(blah)) / (2.0f * a);

  if (t < 0.0f || t > 1.0f)
    return false;

  if (tS) *tS = t;

  return true;
}

//==============================================================
// SegmentCapsuleIntersection
//==============================================================
inline bool SegmentCapsuleIntersection(tScalar* tS, const tSegment& seg, const tCapsule& capsule)
{
  tScalar bestFrac = SCALAR_HUGE;

  // do the main sides
  tScalar sideFrac = SCALAR_HUGE;
  if (!SegmentInfiniteCylinderIntersection(&sideFrac, seg, 
                                           tSegment(capsule.GetPos(), capsule.GetOrient().GetLook()), 
                                           capsule.GetRadius()))
    return false;

  // only keep this if the side intersection point is within the capsule segment ends
  tVector3 sidePos = seg.GetPoint(sideFrac);
  if (Dot((sidePos - capsule.GetPos()), capsule.GetOrient().GetLook()) < 0.0f)
    sideFrac = SCALAR_HUGE;
  else if (Dot((sidePos - capsule.GetEnd()), capsule.GetOrient().GetLook()) > 0.0f)
    sideFrac = SCALAR_HUGE;

  // do the two ends
  tScalar originFrac = SCALAR_HUGE;
  SegmentSphereIntersection(&originFrac, seg, tSphere(capsule.GetPos(), capsule.GetRadius()));
  tScalar endFrac = SCALAR_HUGE;
  SegmentSphereIntersection(&originFrac, seg, tSphere(capsule.GetEnd(), capsule.GetRadius()));

  bestFrac = Min(sideFrac, originFrac, endFrac);
  if (bestFrac <= 1.0f)
  {
    if (tS) *tS = bestFrac;
    return true;
  }
  return false;
}



//====================================================================
// SweptSpherePlaneIntersection
//====================================================================
inline bool SweptSpherePlaneIntersection(tVector3& pt, tScalar& finalPenetration,
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


