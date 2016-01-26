//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file collisionskin.cpp 
//                     
//==============================================================
#include "collisionskin.hpp"
#include "collisionsystem.hpp"
#include "body.hpp"

using namespace JigLib;

//==============================================================
// SetOwner
//==============================================================
void tCollisionSkin::SetOwner(class tBody *owner)
{
  mOwner = owner;
}

//==============================================================
// AddPrimitive
//==============================================================
int tCollisionSkin::AddPrimitive(const tPrimitive &prim, 
				 tMaterialTable::tMaterialID matID, 
				 tMaterialProperties matProps)
{
  tPrimitive *newPrim = prim.Clone();
  if (!newPrim)
  {
    TRACE("tCollisionSkin::AddPrimitive Unable to clone primitive type %d\n", 
	  prim.GetType());
    return -1;
  }
  mPrimitivesOldWorld.push_back(prim.Clone());
  mPrimitivesNewWorld.push_back(prim.Clone());
  mPrimitivesLocal.push_back(newPrim);
  mMaterialIDs.push_back(matID);
  mMaterialProperties.push_back(matProps);

  UpdateWorldBoundingBox();

  return mMaterialIDs.size() - 1;
}

//==============================================================
// RemoveAllPrimitives
//==============================================================
void tCollisionSkin::RemoveAllPrimitives()
{
  for (unsigned iPrim = mPrimitivesNewWorld.size() ; iPrim-- != 0 ; )
  {
    delete mPrimitivesOldWorld[iPrim];
    delete mPrimitivesNewWorld[iPrim];
    delete mPrimitivesLocal[iPrim];
  }
  mPrimitivesOldWorld.clear();
  mPrimitivesNewWorld.clear();
  mPrimitivesLocal.clear();
  mMaterialIDs.clear();
  mMaterialProperties.clear();
}


//==============================================================
// UpdateWorldBoundingBox
//==============================================================
void tCollisionSkin::UpdateWorldBoundingBox()
{
  mWorldBoundingBox.Clear();

  for (unsigned iOld = mPrimitivesOldWorld.size() ; iOld-- != 0 ; )
    mWorldBoundingBox.AddPrimitive(*mPrimitivesOldWorld[iOld]);

  if (mCollSystem && mCollSystem->GetUseSweepTests())
  {
    for (unsigned iNew = mPrimitivesNewWorld.size() ; iNew-- != 0 ; )
      mWorldBoundingBox.AddPrimitive(*mPrimitivesNewWorld[iNew]);
  }
}

//==============================================================
// SetPosAndOrient
//==============================================================
void tCollisionSkin::SetNewTransform(const tTransform3 &transform)
{
  TRACE_METHOD_ONLY(MULTI_FRAME_2);
  mTransformNew = transform;
  tTransform3 t;
  for (unsigned iPrim = mPrimitivesNewWorld.size() ; iPrim-- != 0 ; )
  {
    mPrimitivesLocal[iPrim]->GetTransform(t);
    mPrimitivesNewWorld[iPrim]->SetTransform(transform * t);
  }
  UpdateWorldBoundingBox();
  if (mCollSystem) mCollSystem->CollisionSkinMoved(this);
}

//==============================================================
// SetOldPosAndOrient
//==============================================================
void tCollisionSkin::SetOldTransform(const tTransform3 &transform)
{
  TRACE_METHOD_ONLY(MULTI_FRAME_2);
  mTransformOld = transform;
  tTransform3 t;
  for (unsigned iPrim = mPrimitivesNewWorld.size() ; iPrim-- != 0 ; )
  {
    mPrimitivesLocal[iPrim]->GetTransform(t);
    mPrimitivesOldWorld[iPrim]->SetTransform(transform * t);
  }
  UpdateWorldBoundingBox();
  if (mCollSystem) mCollSystem->CollisionSkinMoved(this);
}

//==============================================================
// SetPosAndOrient
//==============================================================
void tCollisionSkin::SetTransform(const tTransform3 &transformOld, const tTransform3 &transformNew)
{
  TRACE_METHOD_ONLY(MULTI_FRAME_2);
  mTransformOld = transformOld;
  mTransformNew = transformNew;
  tTransform3 t;
  for (unsigned iPrim = mPrimitivesNewWorld.size() ; iPrim-- != 0 ; )
  {
    mPrimitivesLocal[iPrim]->GetTransform(t);
    mPrimitivesOldWorld[iPrim]->SetTransform(transformOld * t);
    mPrimitivesNewWorld[iPrim]->SetTransform(transformNew * t);
  }
  UpdateWorldBoundingBox();
  if (mCollSystem) mCollSystem->CollisionSkinMoved(this);
}

//==============================================================
// SegmentIntersect
//==============================================================
bool tCollisionSkin::SegmentIntersect(tScalar & frac, 
                                      tVector3 & pos, 
                                      tVector3 & normal, 
                                      const tSegment & seg) const
{
  tVector3 segEnd = seg.GetEnd();
  frac = SCALAR_HUGE;
  tScalar thisSegLenRelToOrig = 1.0f;
  tSegment segCopy(seg);

  for (unsigned iPrim = mPrimitivesNewWorld.size() ; iPrim-- != 0 ; )
  {
    tScalar thisFrac;
    if (mPrimitivesNewWorld[iPrim]->SegmentIntersect(thisFrac, pos, normal, segCopy))
    {
      frac = thisFrac * thisSegLenRelToOrig;
      segCopy.mDelta *= thisFrac;
      thisSegLenRelToOrig *= frac;
    }
  }
  
  if (frac <= 1.0f)
    return true;
  else
    return false;
}

//==============================================================
// ~tCollisionSkin
//==============================================================
tCollisionSkin::~tCollisionSkin()
{
  TRACE_METHOD_ONLY(ONCE_2);
  // don't care if this fails
  if (mCollSystem)
    mCollSystem->RemoveCollisionSkin(this);
  if (mOwner)
    mOwner->SetCollisionSkin(0);

  RemoveAllPrimitives();
}

//==============================================================
// tCollisionSkin
//==============================================================
tCollisionSkin::tCollisionSkin(class tBody *owner) : 
mOwner(owner) 
{
  TRACE_METHOD_ONLY(ONCE_2);
  mCollisions.reserve(16);
  mWorldBoundingBox.Clear();
  
  mCollSystem = 0;
}

//==============================================================
// SetMaterialProperties
//==============================================================
void tCollisionSkin::SetMaterialProperties(unsigned iPrim, const tMaterialProperties & matProperties)
{
  mMaterialProperties[iPrim] = matProperties;
  mMaterialIDs[iPrim] = tMaterialTable::USER_DEFINED;
}

//==============================================================
// ApplyLocalTransform
//==============================================================
void tCollisionSkin::ApplyLocalTransform(const tTransform3 &transform)
{
  tTransform3 t;
  for (unsigned iPrim = mPrimitivesNewWorld.size() ; iPrim-- != 0 ; )
  {
    mPrimitivesLocal[iPrim]->GetTransform(t);
    mPrimitivesLocal[iPrim]->SetTransform(transform * t);
  }
  SetTransform(mTransformOld, mTransformNew);
}

//==============================================================
// GetMassProperties
//==============================================================
void tCollisionSkin::GetMassProperties(const tPrimitive::tPrimitiveProperties primitiveProperties[], 
                                       tScalar &mass, 
                                       tVector3 &centerOfMass, 
                                       tMatrix33 &inertiaTensor,
                                       tMatrix33 &inertiaTensorCoM)
{
  mass = 0.0f;
  centerOfMass.SetToZero();
  inertiaTensor.SetTo(0.0f);
  inertiaTensorCoM.SetTo(0.0f);

  for (unsigned iPrim = mPrimitivesLocal.size() ; iPrim-- != 0 ; )
  {
    tScalar m;
    tVector3 com;
    tMatrix33 it;
    mPrimitivesLocal[iPrim]->GetMassProperties(primitiveProperties[iPrim], m, com, it);
    mass += m;
    centerOfMass += m * com;
    inertiaTensor += it;
  }
  if (mass > SCALAR(0.0))
  {
    centerOfMass /= mass;
    // Transfer of axe theorem
    inertiaTensorCoM(0, 0) = inertiaTensor(0, 0) - mass * (Sq(centerOfMass.y) + Sq(centerOfMass.z));
    inertiaTensorCoM(1, 1) = inertiaTensor(1, 1) - mass * (Sq(centerOfMass.z) + Sq(centerOfMass.x));
    inertiaTensorCoM(2, 2) = inertiaTensor(2, 2) - mass * (Sq(centerOfMass.x) + Sq(centerOfMass.y));

    inertiaTensorCoM(0, 1) = inertiaTensorCoM(1, 0) = inertiaTensor(0, 1) + mass * centerOfMass.x * centerOfMass.y;
    inertiaTensorCoM(1, 2) = inertiaTensorCoM(2, 1) = inertiaTensor(1, 2) + mass * centerOfMass.y * centerOfMass.z;
    inertiaTensorCoM(2, 0) = inertiaTensorCoM(0, 2) = inertiaTensor(2, 0) + mass * centerOfMass.z * centerOfMass.x;
  }
}

//==============================================================
// GetMassProperties
//==============================================================
void tCollisionSkin::GetMassProperties(const tPrimitive::tPrimitiveProperties &primitiveProperties, 
                                       tScalar &mass, 
                                       tVector3 &centerOfMass, 
                                       tMatrix33 &inertiaTensor,
                                       tMatrix33 &inertiaTensorCoM)
{
  mass = 0.0f;
  centerOfMass.SetToZero();
  inertiaTensor.SetTo(0.0f);
  inertiaTensorCoM.SetTo(0.0f);

  tScalar totalWeighting = 0.0f;
  if (primitiveProperties.mMassType == tPrimitive::tPrimitiveProperties::MASS)
  {
    for (unsigned iPrim = mPrimitivesLocal.size() ; iPrim-- != 0 ; )
    {
      if (primitiveProperties.mMassDistribution == tPrimitive::tPrimitiveProperties::SOLID)
        totalWeighting += mPrimitivesLocal[iPrim]->GetVolume();
      else
        totalWeighting += mPrimitivesLocal[iPrim]->GetSurfaceArea();
    }
  }

  for (unsigned iPrim = mPrimitivesLocal.size() ; iPrim-- != 0 ; )
  {
    tScalar m;
    tVector3 com;
    tMatrix33 it;

    tPrimitive::tPrimitiveProperties primProperties(primitiveProperties);

    if (primitiveProperties.mMassType == tPrimitive::tPrimitiveProperties::MASS)
    {
      tScalar weighting = 0.0f;
      if (primitiveProperties.mMassDistribution == tPrimitive::tPrimitiveProperties::SOLID)
        weighting = mPrimitivesLocal[iPrim]->GetVolume();
      else
        weighting = mPrimitivesLocal[iPrim]->GetSurfaceArea();
      primProperties.mMassOrDensity *= weighting / totalWeighting;
    }

    mPrimitivesLocal[iPrim]->GetMassProperties(primProperties, m, com, it);
    mass += m;
    centerOfMass += m * com;
    inertiaTensor += it;
  }
  if (mass > SCALAR(0.0))
  {
    centerOfMass /= mass;
    // Transfer of axe theorem
    inertiaTensorCoM(0, 0) = inertiaTensor(0, 0) - mass * (Sq(centerOfMass.y) + Sq(centerOfMass.z));
    inertiaTensorCoM(1, 1) = inertiaTensor(1, 1) - mass * (Sq(centerOfMass.z) + Sq(centerOfMass.x));
    inertiaTensorCoM(2, 2) = inertiaTensor(2, 2) - mass * (Sq(centerOfMass.x) + Sq(centerOfMass.y));

    inertiaTensorCoM(0, 1) = inertiaTensorCoM(1, 0) = inertiaTensor(0, 1) + mass * centerOfMass.x * centerOfMass.y;
    inertiaTensorCoM(1, 2) = inertiaTensorCoM(2, 1) = inertiaTensor(1, 2) + mass * centerOfMass.y * centerOfMass.z;
    inertiaTensorCoM(2, 0) = inertiaTensorCoM(0, 2) = inertiaTensor(2, 0) + mass * centerOfMass.z * centerOfMass.x;
  }

  if (primitiveProperties.mMassType == tPrimitive::tPrimitiveProperties::MASS)
    mass = primitiveProperties.mMassOrDensity;
}

//==============================================================
// GetVolume
//==============================================================
tScalar tCollisionSkin::GetVolume() const
{
  tScalar result = 0.0f;
  for (unsigned iPrim = mPrimitivesLocal.size() ; iPrim-- != 0 ; )
    result += mPrimitivesLocal[iPrim]->GetVolume();
  return result;
}

//==============================================================
// GetSurfaceArea
//==============================================================
tScalar tCollisionSkin::GetSurfaceArea() const
{
  tScalar result = 0.0f;
  for (unsigned iPrim = mPrimitivesLocal.size() ; iPrim-- != 0 ; )
    result += mPrimitivesLocal[iPrim]->GetSurfaceArea();
  return result;
}

