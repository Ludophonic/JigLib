//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file capsuleobject.cpp 
//                     
//==============================================================
#include "capsuleobject.hpp"
#include "debugconfig.hpp"

using namespace JigLib;

//==============================================================
// SetProperties
//==============================================================
void tCapsuleObject::SetProperties(tScalar elasticity, tScalar staticFriction, tScalar dynamicFriction)
{
  mCollisionSkin.SetMaterialProperties(0, tMaterialProperties(elasticity, staticFriction, dynamicFriction));
}

//==============================================================
// tCapsuleObject
//==============================================================
tCapsuleObject::tCapsuleObject(tScalar radius, tScalar length, const tMatrix33 & orient)
{
  TRACE_METHOD_ONLY(ONCE_2);
  
  mCollisionSkin.AddPrimitive(tCapsule(orient * tVector3(-0.5f * length, 0.0f, 0.0f), orient, radius, length), 
    tMaterialTable::UNSET);
  mCollisionSkin.SetOwner(&mBody);
  SetProperties(0.6f, 0.8f, 0.6f);
  
  mBody.SetCollisionSkin(&mCollisionSkin);
  SetMass(1.0f);
  
  mDisplayListNum = 0;
}

//==============================================================
// ~tCapsuleObject
//==============================================================
tCapsuleObject::~tCapsuleObject()
{
  TRACE_METHOD_ONLY(ONCE_2);
}

//==============================================================
// SetDensity
//==============================================================
void tCapsuleObject::SetDensity(tScalar density, tPrimitive::tPrimitiveProperties::tMassDistribution massType)
{
  SetMass(density * mCollisionSkin.GetVolume(), massType);
}

//==============================================================
// SetMass
//==============================================================
void tCapsuleObject::SetMass(tScalar mass, tPrimitive::tPrimitiveProperties::tMassDistribution massType)
{
  mBody.SetMass(mass);
  tPrimitive::tPrimitiveProperties primitiveProperties(massType, 
    tPrimitive::tPrimitiveProperties::MASS, mass);
  tScalar junk;
  tVector3 com;
  tMatrix33 it, itCoM;
  mCollisionSkin.GetMassProperties(primitiveProperties, junk, com, it, itCoM);

  bool alignBodyWithGeometry = false;
  if (alignBodyWithGeometry)
  {
    // move body to geometry
    mBody.MoveTo(mBody.GetPosition() + com, mBody.GetOrientation());
    mCollisionSkin.ApplyLocalTransform(tTransform3(-com, tMatrix33::Identity()));
    mBody.SetBodyInertia(itCoM);
  }
  else
  {
    mBody.SetBodyInertia(it);
  }
}


//==============================================================
// SetRenderPosition
//==============================================================
void tCapsuleObject::SetRenderPosition(tScalar renderFraction)
{
  TRACE_METHOD_ONLY(FRAME_2);
  if (renderFraction < SCALAR_TINY)
  {
    mRenderPosition = mBody.GetOldPosition();
    mRenderOrientation = mBody.GetOldOrientation();
  }
  else if (renderFraction > (1.0f - SCALAR_TINY))
  {
    mRenderPosition = mBody.GetPosition();
    mRenderOrientation = mBody.GetOrientation();
  }
  else
  {
    mRenderPosition = 
      mBody.GetPosition() * renderFraction + 
      mBody.GetOldPosition() * (1.0f - renderFraction);
    // should really use quaternions and SLERP... but maybe if the
    // object is spinning so fast then it doesn't matter if it renders
    // a bit wrong...
    mRenderOrientation = 
      mBody.GetOrientation() * renderFraction + 
      mBody.GetOldOrientation() * (1.0f - renderFraction);
//    mRenderOrientation.Orthonormalise();
  }
}

//==============================================================
// GetRenderBoundingCapsule
//==============================================================
const tSphere & tCapsuleObject::GetRenderBoundingSphere() const
{
  // fudge this a little...
  const tCapsule & capsule = mCollisionSkin.GetPrimitiveNewWorld(0)->GetCapsule();
  static tSphere sphere;
  sphere.SetPos(capsule.GetPos() + 0.5f * capsule.GetLength() * capsule.GetOrient().GetLook());
  sphere.SetRadius(capsule.GetRadius() + 0.5f * capsule.GetLength());
  return sphere;
}

//==============================================================
// GetRenderPosition
//==============================================================
const tVector3 & tCapsuleObject::GetRenderPosition() const
{
  return mRenderPosition;
}

//==============================================================
// GetRenderOrientation
//==============================================================
const tMatrix33 & tCapsuleObject::GetRenderOrientation() const
{
  return mRenderOrientation;
}

//==============================================================
// Render
//==============================================================
void tCapsuleObject::Render(tRenderObject::tRenderType renderType)
{
  TRACE_METHOD_ONLY(FRAME_2);
  const tCapsule & capsule = mCollisionSkin.GetPrimitiveLocal(0)->GetCapsule();
  
  if (mDisplayListNum == 0)
  {
    mDisplayListNum = glGenLists(1);
    glNewList(mDisplayListNum, GL_COMPILE);
    GLCOLOR4(RangedRandom(0.2f, 1.0f), 
             RangedRandom(0.2f, 1.0f),
             RangedRandom(0.2f, 1.0f), 
             0.5f);

    unsigned num = 12;

    ApplyTransformation(capsule.GetPos(), capsule.GetOrient());
    GLROTATE(90.0f, 0.0f, 1.0f, 0.0f);
    gluSphere(GetGLUQuadric(), 
              capsule.GetRadius(), 
              num, num);
    gluCylinder(GetGLUQuadric(), capsule.GetRadius(), capsule.GetRadius(), capsule.GetLength(), num, num);

    GLTRANSLATE(0.0f, 0.0f, capsule.GetLength());
    gluSphere(GetGLUQuadric(), 
              capsule.GetRadius(), 
              num, num);

    glEndList();
  }
  
  if (tDebugConfig::mRenderCollisionBoundingBoxes)
    RenderWorldBoundingBox();
  
  ApplyTransformation(mRenderPosition, mRenderOrientation);
  bool translucent = !mBody.IsActive();
  if (translucent && tDebugConfig::mIndicateFrozenObjects)
  {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }
  
  glCallList(mDisplayListNum);
  
  if (translucent && tDebugConfig::mIndicateFrozenObjects)
    glDisable(GL_BLEND);
}

