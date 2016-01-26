//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file sphere.cpp 
//                     
//==============================================================
#include "sphere.hpp"
#include "intersection.hpp"

using namespace JigLib;

tSphere tSphere::mHugeSphere(tVector3(0.0f), SCALAR_HUGE);

//==============================================================
// Clone
//==============================================================
tPrimitive* tSphere::Clone() const
{
  return new tSphere(*this);
}

//==============================================================
// SegmentIntersect
//==============================================================
bool tSphere::SegmentIntersect(tScalar &frac, tVector3 &pos, tVector3 &normal, const class tSegment &seg) const
{
  bool result;
  if (result = SegmentSphereIntersection(&frac, seg, *this))
  {
    pos = seg.GetPoint(frac);
    normal = pos - mPos;
    normal.NormaliseSafe(); // TODO could just divide by radius...?
  }
  return result;
}

//==============================================================
// GetMassProperties
//==============================================================
void tSphere::GetMassProperties(const tPrimitiveProperties &primitiveProperties, 
                               tScalar &mass, 
                               tVector3 &centerOfMass, 
                               tMatrix33 &inertiaTensor) const
{
  if (primitiveProperties.mMassType == tPrimitiveProperties::MASS)
  {
    mass = primitiveProperties.mMassOrDensity;
  }
  else
  {
    if (primitiveProperties.mMassDistribution == tPrimitiveProperties::SOLID)
      mass = GetVolume() * primitiveProperties.mMassOrDensity;
    else
      mass = GetSurfaceArea() * primitiveProperties.mMassOrDensity;
  }

  centerOfMass = mPos;
  tScalar Ixx;
  if (primitiveProperties.mMassDistribution == tPrimitiveProperties::SOLID)
    Ixx = 0.2f * mass * GetRadius();
  else
    Ixx = (2.0f / 3.0f) * mass * GetRadius();
    
  inertiaTensor.Set(Ixx, 0.0f, 0.0f,
                    0.0f, Ixx, 0.0f,
                    0.0f, 0.0f, Ixx);

  // Transfer of axe theorem
  inertiaTensor(0, 0) = inertiaTensor(0, 0) + mass * (Sq(centerOfMass.y) + Sq(centerOfMass.z));
  inertiaTensor(1, 1) = inertiaTensor(1, 1) + mass * (Sq(centerOfMass.z) + Sq(centerOfMass.x));
  inertiaTensor(2, 2) = inertiaTensor(2, 2) + mass * (Sq(centerOfMass.x) + Sq(centerOfMass.y));

  inertiaTensor(0, 1) = inertiaTensor(1, 0) = inertiaTensor(0, 1) - mass * centerOfMass.x * centerOfMass.y;
  inertiaTensor(1, 2) = inertiaTensor(2, 1) = inertiaTensor(1, 2) - mass * centerOfMass.y * centerOfMass.z;
  inertiaTensor(2, 0) = inertiaTensor(0, 2) = inertiaTensor(2, 0) - mass * centerOfMass.z * centerOfMass.x;
}

