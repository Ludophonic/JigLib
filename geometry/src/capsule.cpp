//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file capsule.cpp 
//                     
//==============================================================
#include "capsule.hpp"
#include "intersection.hpp"

using namespace JigLib;

//==============================================================
// Clone
//==============================================================
tPrimitive* tCapsule::Clone() const
{
  return new tCapsule(*this);
}

//==============================================================
// SegmentIntersect
//==============================================================
bool tCapsule::SegmentIntersect(tScalar &frac, tVector3 &pos, tVector3 &normal, const class tSegment &seg) const
{
  bool result;
  if (result = SegmentCapsuleIntersection(&frac, seg, *this))
  {
    pos = seg.GetPoint(frac);
    normal = pos - mTransform.position;
    normal -= Dot(normal, mTransform.orientation.GetLook()) * mTransform.orientation.GetLook();
    normal.NormaliseSafe();
  }
  return result;
}

//==============================================================
// GetMassProperties
//==============================================================
void tCapsule::GetMassProperties(const tPrimitiveProperties &primitiveProperties, 
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

  centerOfMass = GetPos() + 0.5f * GetLength() * GetOrient().GetLook();

  /// todo check solid/shell
  // first cylinder
  tScalar cylinderMass = mass * PI * Sq(mRadius) * mLength / GetVolume();
  tScalar Ixx = 0.5f * cylinderMass * Sq(mRadius);
  tScalar Iyy = 0.25f * cylinderMass * Sq(mRadius) + (1.0f / 12.0f) * cylinderMass * Sq(mLength);
  tScalar Izz = Iyy;
  // add ends
  tScalar endMass = mass - cylinderMass;
  Ixx += 0.2f * endMass * Sq(mRadius);
  Iyy += 0.4f * endMass * Sq(mRadius) + endMass * Sq(0.5f * mLength);
  Izz += 0.4f * endMass * Sq(mRadius) + endMass * Sq(0.5f * mLength);

  inertiaTensor.Set(Ixx, 0.0f, 0.0f,
                    0.0f, Iyy, 0.0f,
                    0.0f, 0.0f, Izz);
  // transform - e.g. see p664 of Physics-Based Animation
  // todo is the order correct here? Does it matter?

  // Calculate the tensor in a frame at the CoM, but aligned with the world axes
  inertiaTensor = mTransform.orientation * inertiaTensor * mTransform.orientation.GetTranspose();

  // Transfer of axe theorem
  inertiaTensor(0, 0) = inertiaTensor(0, 0) + mass * (Sq(centerOfMass.y) + Sq(centerOfMass.z));
  inertiaTensor(1, 1) = inertiaTensor(1, 1) + mass * (Sq(centerOfMass.z) + Sq(centerOfMass.x));
  inertiaTensor(2, 2) = inertiaTensor(2, 2) + mass * (Sq(centerOfMass.x) + Sq(centerOfMass.y));

  inertiaTensor(0, 1) = inertiaTensor(1, 0) = inertiaTensor(0, 1) - mass * centerOfMass.x * centerOfMass.y;
  inertiaTensor(1, 2) = inertiaTensor(2, 1) = inertiaTensor(1, 2) - mass * centerOfMass.y * centerOfMass.z;
  inertiaTensor(2, 0) = inertiaTensor(0, 2) = inertiaTensor(2, 0) - mass * centerOfMass.z * centerOfMass.x;
}

