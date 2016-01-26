//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file plane.cpp 
//                     
//==============================================================
#include "plane.hpp"
#include "intersection.hpp"

using namespace JigLib;

tPrimitive* tPlane::Clone() const
{
  return new tPlane(*this);
}

//==============================================================
// SegmentIntersect
//==============================================================
bool tPlane::SegmentIntersect(tScalar &frac, tVector3 &pos, tVector3 &normal, const class tSegment &seg) const
{
  bool result;
  if (result = SegmentPlaneIntersection(&frac, seg, *this))
  {
    pos = seg.GetPoint(frac);
    normal = GetN();
  }
  return result;
}

//==============================================================
// GetMassProperties
//==============================================================
void tPlane::GetMassProperties(const tPrimitiveProperties &primitiveProperties, 
                               tScalar &mass, 
                               tVector3 &centerOfMass, 
                               tMatrix33 &inertiaTensor) const
{
  mass = 0.0f;
  centerOfMass.SetToZero();
  inertiaTensor.SetTo(0.0f);
}

