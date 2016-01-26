//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file aabox.inl 
//                     
//==============================================================
#include "../utils/include/assert.hpp"

//==============================================================
// Clear
//==============================================================
inline void tAABox::Clear() {
  mMinPos.SetTo(SCALAR_HUGE); mMaxPos.SetTo(-SCALAR_HUGE);}

//==============================================================
// tAABox
//==============================================================
inline tAABox::tAABox(bool clear) :
  tPrimitive(tPrimitive::AABOX)
{
  if (clear) Clear();
}

//==============================================================
// AddPoint
//==============================================================
inline void tAABox::AddPoint(const tVector3 & pos)
{
  if (pos.x < mMinPos.x) mMinPos.x = pos.x - SCALAR_TINY;
  if (pos.x > mMaxPos.x) mMaxPos.x = pos.x + SCALAR_TINY;

  if (pos.y < mMinPos.y) mMinPos.y = pos.y - SCALAR_TINY;
  if (pos.y > mMaxPos.y) mMaxPos.y = pos.y + SCALAR_TINY;

  if (pos.z < mMinPos.z) mMinPos.z = pos.z - SCALAR_TINY;
  if (pos.z > mMaxPos.z) mMaxPos.z = pos.z + SCALAR_TINY;
}

//==============================================================
// AddSphere
//==============================================================
inline void tAABox::AddSphere(const tSphere & sphere)
{
  for (unsigned i = 0 ; i < 3 ; ++i)
  {
    if ((sphere.GetPos()[i] - sphere.GetRadius()) < mMinPos[i])
      mMinPos[i] = (sphere.GetPos()[i] - sphere.GetRadius()) - SCALAR_TINY;
    if ((sphere.GetPos()[i] + sphere.GetRadius()) > mMaxPos[i])
      mMaxPos[i] = (sphere.GetPos()[i] + sphere.GetRadius()) + SCALAR_TINY;
  }
}

//==============================================================
// AddBox
//==============================================================
inline void tAABox::AddBox(const tBox & box)
{
  tVector3 pts[8];
  box.GetCornerPoints(pts);
  AddPoint(pts[0]);
  AddPoint(pts[1]);
  AddPoint(pts[2]);
  AddPoint(pts[3]);
  AddPoint(pts[4]);
  AddPoint(pts[5]);
  AddPoint(pts[6]);
  AddPoint(pts[7]);
}

//==============================================================
// AddSegment
//==============================================================
inline void tAABox::AddSegment(const tSegment & seg)
{
  AddPoint(seg.mOrigin);
  AddPoint(seg.GetEnd());
}

//==============================================================
// AddCapsule
//==============================================================
inline void tAABox::AddCapsule(const tCapsule & capsule)
{
  AddSphere(tSphere(capsule.GetPos(), capsule.GetRadius()));
  AddSphere(tSphere(capsule.GetPos() + capsule.GetLength() * capsule.GetOrient().GetLook(), capsule.GetRadius()));
}

//==============================================================
// AddAABox
//==============================================================
inline void tAABox::AddAABox(const tAABox &aabox)
{
  AddPoint(aabox.GetMaxPos());
  AddPoint(aabox.GetMinPos());
}


//==============================================================
// IsPointInside
//==============================================================
inline bool tAABox::IsPointInside(const tVector3 & pos) const
{
  return ( (pos.x >= mMinPos.x) && 
           (pos.x <= mMaxPos.x) &&
           (pos.y >= mMinPos.y) && 
           (pos.y <= mMaxPos.y) &&
           (pos.z >= mMinPos.z) && 
           (pos.z <= mMaxPos.z) );
}

//==============================================================
// OverlapTest
//==============================================================
inline bool OverlapTest(const tAABox & box0, const tAABox & box1)
{
  return ( 
    (box0.mMinPos.z >= box1.mMaxPos.z) ||
    (box0.mMaxPos.z <= box1.mMinPos.z) ||
    (box0.mMinPos.y >= box1.mMaxPos.y) ||
    (box0.mMaxPos.y <= box1.mMinPos.y) ||
    (box0.mMinPos.x >= box1.mMaxPos.x) ||
    (box0.mMaxPos.x <= box1.mMinPos.x) ) ? false : true;
}

//==============================================================
// OverlapTest
//==============================================================
inline bool OverlapTest(const tAABox & box0, const tAABox & box1, tScalar tol)
{
  return ( 
    (box0.mMinPos.z >= box1.mMaxPos.z + tol) ||
    (box0.mMaxPos.z <= box1.mMinPos.z - tol) ||
    (box0.mMinPos.y >= box1.mMaxPos.y + tol) ||
    (box0.mMaxPos.y <= box1.mMinPos.y - tol) ||
    (box0.mMinPos.x >= box1.mMaxPos.x + tol) ||
    (box0.mMaxPos.x <= box1.mMinPos.x - tol) ) ? false : true;
}
