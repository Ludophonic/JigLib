//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file plane.inl 
//                     
//==============================================================
// should only be included from plane.hpp

//==============================================================
// Set
//==============================================================
inline void tPlane::Set(const tVector3 & n, const tScalar d)
{
  mN = n;
  mD = d;
}

//==============================================================
// Set
//==============================================================
inline void tPlane::Set(const tVector3 & n, const tVector3 & pos)
{
  mN = n.GetNormalisedSafe();
  mD = -Dot(mN, pos);
}

//==============================================================
// Set
//==============================================================
inline void tPlane::Set(const tVector3 & pos0, 
                        const tVector3 & pos1, 
                        const tVector3 & pos2)
{
  tVector3 dr1 = pos1 - pos0;
  tVector3 dr2 = pos2 - pos0;
  
  mN = Cross(dr1, dr2);
  tScalar mNLen = mN.GetLength();
  if (mNLen < SCALAR_TINY)
  {
    Assert(!"co-linear points when setting a plane");
    // could handle this...
    mN = tVector3::Up();
    mD = 0.0f;
  }
  else
  {
    mN /= mNLen;
    mD = -Dot(mN, pos0);
  }
}

//==============================================================
// tPlane
//==============================================================
inline tPlane::tPlane(const tVector3 & n, const tScalar d)  : tPrimitive(tPrimitive::PLANE) 
{
  Set(n, d);
}

//==============================================================
// tPlane
//==============================================================
inline tPlane::tPlane(const tVector3 & n, const tVector3 & pos)  : tPrimitive(tPrimitive::PLANE) 
{
  Set(n, pos);
}

//==============================================================
// tPlane
//==============================================================
inline tPlane::tPlane(const tVector3 & pos0, 
                      const tVector3 & pos1, 
                      const tVector3 & pos2)
: tPrimitive(tPrimitive::PLANE) 
{
  Set(pos0, pos1, pos2);
}

//==============================================================
// GetDistanceToPoint
//==============================================================
inline tScalar tPlane::GetDistanceToPoint(const tVector3 & pos) const
{
  return Dot(mN, pos) + mD;
}

//==============================================================
// Transform
//==============================================================
inline tPlane & tPlane::Transform(const tMatrix33 & mat, 
                                  const tVector3 & vec)
{
  Assert(!"Not implemented");
  return *this;
/*  
    tVector3 vec2 = -( mat.GetTranspose() * vec);
    cVector4 p(n(), d());
    cMatrix44 invTransT = cMatrix44(mat.GetTranspose(), grrr).GetTranspose();
    cVector4 np = p * invTransT;
    Set(np[0], np[1], np[2], np[3]);
    return *this;
*/
}

//==============================================================
// GetTransform
//==============================================================
inline tPlane tPlane::GetTransform(const tMatrix33 & mat, 
                                   const tVector3 & vec) const
{
  return tPlane(*this).Transform(mat, vec);
}

//==============================================================
// GetLineIntersection
//==============================================================
inline bool tPlane::GetLineIntersection(tScalar & t,
                                        const tLine & line) const
{
  tScalar dot = Dot(line.mDir, mN);
  if (Abs(dot) < SCALAR_TINY)
    return false;
  
  tScalar dist = GetDistanceToPoint(line.mOrigin);
  t = -dist / dot;
  return true;
}


//==============================================================
// GetRayIntersection
//==============================================================
inline bool tPlane::GetRayIntersection(tScalar & t,
                                       const tRay & ray) const
{
  tScalar dot = Dot(ray.mDir, mN);
  if (Abs(dot) < SCALAR_TINY)
    return false;
  
  tScalar dist = GetDistanceToPoint(ray.mOrigin);
  t = -dist / dot;
  if (t >= 0.0f)
    return true;
  else
    return false;
}

//==============================================================
// GetSegmentIntersection
//==============================================================
inline bool tPlane::GetSegmentIntersection(tScalar & t,
                                           const tSegment & segment) const
{
  tScalar dot = Dot(segment.mDelta, mN);
  if (Abs(dot) < SCALAR_TINY)
    return false;
  
  tScalar dist = GetDistanceToPoint(segment.mOrigin);
  t = -dist / dot;
  if ( (t >= 0.0f) && (t <= 1.0f) )
    return true;
  else
    return false;
}
