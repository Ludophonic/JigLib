//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file aabox.cpp 
//                     
//==============================================================
#include "aabox.hpp"

using namespace JigLib;

tAABox tAABox::mHugeBox(tVector3(-SCALAR_HUGE), tVector3(SCALAR_HUGE));

//==============================================================
// Clone
//==============================================================
tPrimitive* tAABox::Clone() const
{
  return new tAABox(*this);
}

//==============================================================
// SegmentIntersect
//==============================================================
bool tAABox::SegmentIntersect(tScalar &frac, tVector3 &pos, tVector3 &normal, const class tSegment &seg) const
{
  /// todo implement
  Assert(!"Not implemented");
  return false;
}

//==============================================================
// GetMassProperties
//==============================================================
void tAABox::GetMassProperties(const tPrimitiveProperties &primitiveProperties, 
                               tScalar &mass, 
                               tVector3 &centerOfMass, 
                               tMatrix33 &inertiaTensor) const
{
  mass = 0.0f;
  centerOfMass.SetToZero();
  inertiaTensor.SetTo(0.0f);
}


//==============================================================
// AddPrimitive
//==============================================================
void tAABox::AddPrimitive(const tPrimitive &prim)
{
  switch (prim.GetType())
  {
  case tPrimitive::AABOX:
    AddAABox(prim.GetAABox()); break;
  case tPrimitive::BOX:
    AddBox(prim.GetBox()); break;
  case tPrimitive::CAPSULE:
    AddCapsule(prim.GetCapsule()); break;
  case tPrimitive::SPHERE:
    AddSphere(prim.GetSphere()); break;
  case tPrimitive::PLANE:
    *this = HugeBox();
  default:
    AddAABox(prim.GetBoundingBox());
    break;
  }
}

