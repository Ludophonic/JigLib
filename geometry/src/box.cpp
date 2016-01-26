//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file box.cpp 
//                     
//==============================================================
#include "box.hpp"
#include "line.hpp"
#include "trace.hpp"

using namespace JigLib;

// must match with GetCornerPoints!
const tBox::tEdge tBox::mEdges[12] = 
{
  {BRD, BRU}, // origin-up
  {BRD, BLD}, // origin-left
  {BRD, FRD}, // origin-fwd
  {BLD, BLU}, // leftorigin-up
  {BLD, FLD}, // leftorigin-fwd
  {FRD, FRU}, // fwdorigin-up
  {FRD, FLD}, // fwdorigin-left
  {BRU, BLU}, // uporigin-left
  {BRU, FRU}, // uporigin-fwd
  {BLU, FLU}, // upleftorigin-fwd
  {FRU, FLU}, // upfwdorigin-left
  {FLD, FLU}, // fwdleftorigin-up
};

//==============================================================
// Clone
//==============================================================
tPrimitive* tBox::Clone() const
{
  return new tBox(*this);
}

//==============================================================
// GetSqDistanceToPoint
//==============================================================
tScalar tBox::GetSqDistanceToPoint(tVector3 & closestBoxPoint,
                                   const tVector3 & point) const
{
  TRACE_METHOD_ONLY(MULTI_FRAME_3);
  
  // compute coordinates of point in box coordinate system
  closestBoxPoint = mTransform.orientation.GetTranspose() * (point - mTransform.position);
  
  // project test point onto box
  tScalar sqDistance = SCALAR(0.0f);
  tScalar delta;
  
  if ( closestBoxPoint.x < SCALAR(0.0f) )
  {
    sqDistance += Sq(closestBoxPoint.x);
    closestBoxPoint.x = SCALAR(0.0f);
  }
  else if ( closestBoxPoint.x > mSideLengths.x )
  {
    delta = closestBoxPoint.x - mSideLengths.x;
    sqDistance += Sq(delta);
    closestBoxPoint.x = mSideLengths.x;
  }
  
  if ( closestBoxPoint.y < SCALAR(0.0f) )
  {
    sqDistance += Sq(closestBoxPoint.y);
    closestBoxPoint.y = SCALAR(0.0f);
  }
  else if ( closestBoxPoint.y > mSideLengths.y )
  {
    delta = closestBoxPoint.y - mSideLengths.y;
    sqDistance += Sq(delta);
    closestBoxPoint.y = mSideLengths.y;
  }
  
  if ( closestBoxPoint.z < SCALAR(0.0f) )
  {
    sqDistance += Sq(closestBoxPoint.z);
    closestBoxPoint.z = SCALAR(0.0f);
  }
  else if ( closestBoxPoint.z > mSideLengths.z )
  {
    delta = closestBoxPoint.z - mSideLengths.z;
    sqDistance += Sq(delta);
    closestBoxPoint.z = mSideLengths.z;
  }
  closestBoxPoint = mTransform.position + mTransform.orientation * closestBoxPoint;
  return sqDistance;
}

//==============================================================
// SegmentIntersect
//==============================================================
bool tBox::SegmentIntersect(tScalar & fracOut, 
                            tVector3 & posOut, 
                            tVector3 & normalOut, 
                            const tSegment & seg) const
{
  fracOut = SCALAR_HUGE;

  // algo taken from p674 of realting rendering
  // needs debugging
  tScalar tMin = -SCALAR_HUGE;
  tScalar tMax = SCALAR_HUGE;
  tVector3 p = GetCentre() - seg.mOrigin;
  tVector3 h = mSideLengths * 0.5f;
  unsigned iDirMax = 0;
  unsigned iDirMin = 0;
  unsigned iDir;
  
  for (iDir = 0 ; iDir < 3 ; ++iDir)
  {
    tScalar e = Dot(mTransform.orientation[iDir], p);
    tScalar f = Dot(mTransform.orientation[iDir], seg.mDelta);
    if (Abs(f) > SCALAR_TINY)
    {
      tScalar t1 = (e + h[iDir]) / f;
      tScalar t2 = (e - h[iDir]) / f;
      if (t1 > t2) Swap(t1, t2);
      if (t1 > tMin) 
      {
        tMin = t1; 
        iDirMin = iDir;
      }
      if (t2 < tMax) 
      {
        tMax = t2; 
        iDirMax = iDir;
      }
      if (tMin > tMax) 
        return false;
      if (tMax < SCALAR(0.0f)) 
        return false;
    }
    else if ( (-e - h[iDir] > SCALAR(0.0f)) ||
              (-e + h[iDir] < SCALAR(0.0f)) )
    {
      return false;
    }
  }
  if (tMin > SCALAR(0.0f))
  {
    iDir = iDirMin;
    fracOut = tMin;
  }
  else
  {
    iDir = iDirMax;
    fracOut = tMax;
  }
  Limit(fracOut, SCALAR(0.0f), SCALAR(1.0f));
  posOut = seg.GetPoint(fracOut);
  if (Dot(mTransform.orientation[iDir], seg.mDelta) > SCALAR(0.0f))
    normalOut = -mTransform.orientation[iDir];
  else
    normalOut = mTransform.orientation[iDir];
  return true;
}

//==============================================================
// GetMassProperties
//==============================================================
void tBox::GetMassProperties(const tPrimitiveProperties &primitiveProperties, 
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

  centerOfMass = GetCentre();
  /// First calculate inertia in local frame, then shift to origin
  const tVector3 & sides = GetSideLengths();
  /// todo check solid/shell
  tScalar Ixx = (1.0f / 12.0f) * mass * 
    (Sq(sides.y) + Sq(sides.z));
  tScalar Iyy = (1.0f / 12.0f) * mass * 
    (Sq(sides.x) + Sq(sides.z));
  tScalar Izz = (1.0f / 12.0f) * mass * 
    (Sq(sides.x) + Sq(sides.y));
  
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

