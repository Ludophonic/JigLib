//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file distance.inl 
//                     
//==============================================================

//==============================================================
// PointPointDistanceSq
//==============================================================
inline tScalar PointPointDistanceSq(const tVector3 & pt1, const tVector3 & pt2)
{
  return Sq(pt2.x - pt1.x) + Sq(pt2.y - pt1.y) + Sq(pt2.z - pt1.z);
}

//==============================================================
// PointPointDistance
//==============================================================
inline tScalar PointPointDistance(const tVector3 & pt1, const tVector3 & pt2)
{
  return Sqrt(Sq(pt2.x - pt1.x) + Sq(pt2.y - pt1.y) + Sq(pt2.z - pt1.z));
}

//==============================================================
// SegmentPointDistanceSq
//==============================================================
inline tScalar PointSegmentDistanceSq(tScalar * t, const tVector3 & pt, const tSegment & seg)
{
  tVector3 kDiff = pt - seg.mOrigin;
  tScalar fT = Dot(kDiff, seg.mDelta);

  if ( fT <= SCALAR(0.0f) )
  {
    fT = SCALAR(0.0f);
  }
  else
  {
    tScalar fSqrLen= seg.mDelta.GetLengthSq();
    if ( fT >= fSqrLen )
    {
      fT = SCALAR(1.0f);
      kDiff -= seg.mDelta;
    }
    else
    {
      fT /= fSqrLen;
      kDiff -= fT*seg.mDelta;
    }
  }

  if ( t )
    *t = fT;

  return kDiff.GetLengthSq();

}

//==============================================================
// SegmentPointDistance
//==============================================================
inline tScalar PointSegmentDistance(tScalar * t, const tVector3 & pt, const tSegment & seg)
{
  return Sqrt(PointSegmentDistanceSq(t, pt, seg));
}

//==============================================================
// PointPlaneDistance
//==============================================================
inline tScalar PointPlaneDistance(const tVector3 & pt, const tPlane & plane)
{
  return Dot(plane.GetN(), pt) + plane.GetD();
}


//==============================================================
// SegmentSegmentDistance
//==============================================================
inline tScalar SegmentSegmentDistance(tScalar *t0, tScalar *t1, 
                                      const tSegment & seg0, const tSegment & seg1)
{
  return Sqrt(SegmentSegmentDistanceSq(t0, t1, seg0, seg1));
}

